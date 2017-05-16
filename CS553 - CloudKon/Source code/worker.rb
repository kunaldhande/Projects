#!/usr/bin/env ruby

require 'aws-sdk'
require 'time_difference'

# AWS CONFIG
Aws.config.update({
  region: 'us-west-2',
  credentials: Aws::Credentials.new('AKIAJFNYLH57Z6XUN6IQ', 'POnGJPjpFop/ZH0xRCx/qfVBknGvC9Cl2Ke/bmqR')
})

if(ARGV.length!=2)
  abort("Enter parameters corrently\nworker –s QNAME –t N")
end
  
@queuename = ARGV[0]
@noOfThreads = ARGV[1].to_i


@dynamoDB = Aws::DynamoDB::Client.new
@queue = Queue.new
@run = true
@output = Array.new


# Delete meeage_id from DynamoDB
def delete_item(message_id)
  resp = @dynamoDB.delete_item({
    table_name: "Messages", # required
    key: { # required
      "message_id" => message_id, # value <Hash,Array,String,Numeric,Boolean,IO,Set,nil>
    }
  })
end

# Check message_id present or not
def get_item(message_id)
  resp = @dynamoDB.get_item({
    table_name: "Messages", # required
    key: { # required
      "message_id" => message_id, # value <Hash,Array,String,Numeric,Boolean,IO,Set,nil>
    }
  })
  return resp.item ? true : false
end


# Getting task from SQS
def getMessagesFromSQS()
  sqs = Aws::SQS::Client.new
  queue = sqs.create_queue({:queue_name=>@queuename})
  
  poller = Aws::SQS::QueuePoller.new(queue.queue_url)
  poller.poll(idle_timeout: 20) do |msg|
    @queue << msg
    while(true)
      if(@queue.length() < @noOfThreads)
        break
      end
    end
  end
  @run = false
end
  
# Thread Function
def performOperations()
  while @run do
    if @queue.length()>0
      msg = @queue.pop
      
      begin
        if(get_item(msg.message_id))
          output = `#{msg.body}`  
          @output << "#{msg.message_id}\t#{output}\n"
          delete_item(msg.message_id)
        end
      rescue
        @output << "#{msg.message_id}\tfalse\n"
      end
      
    end
  end
end

threads = Array.new

start_time = Time.now

threads << Thread.new{getMessagesFromSQS()}
sleep(3)
puts "performing operations: "
for i in 1..@noOfThreads
  threads << Thread.new{performOperations()}
end

threads.each { |thr| thr.join }

end_time = Time.now
puts "Time: #{TimeDifference.between(start_time, end_time).in_seconds} Sec"

File.open("output.txt", 'w') { |file| file.write(@output.join) }