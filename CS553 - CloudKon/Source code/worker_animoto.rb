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

def delete_item(message_id)
  resp = @dynamoDB.delete_item({
    table_name: "Messages", # required
    key: { # required
      "message_id" => message_id, # value <Hash,Array,String,Numeric,Boolean,IO,Set,nil>
    }
    })
end

def get_item(message_id)
  resp = @dynamoDB.get_item({
    table_name: "Messages", # required
    key: { # required
      "message_id" => message_id, # value <Hash,Array,String,Numeric,Boolean,IO,Set,nil>
    }
  })
  return resp.item ? true : false
end

def saveToS3(message_id)
    s3 = Aws::S3::Resource.new(region:'us-west-2')
    obj = s3.bucket("dsbajvbdshvbewfbv").object("#{message_id}").upload_file("/tmp/#{message_id}/out.mp4")
    return "https://s3-us-west-2.amazonaws.com/dsbajvbdshvbewfbv/#{message_id}"
end   

def getMessagesFromSQS()
  sqs = Aws::SQS::Client.new
  queue = sqs.create_queue({:queue_name=>@queuename})
  
  poller = Aws::SQS::QueuePoller.new(queue.queue_url)
  poller.poll(idle_timeout: 500) do |msg|
    @queue << msg
    while(true)
      if(@queue.length() < @noOfThreads)
        break
      else
        sleep(1)
      end
    end
  end
  @run = false
end

def createVideo(message, message_id)
  begin
    `mkdir /tmp/#{message_id}`
    message.split(',').map.with_index  { |line, i|  `wget -q #{line.strip + " -O /tmp/#{message_id}/img%03d.jpg" % i}` }

    `ffmpeg -loglevel panic -framerate 1 -i /tmp/#{message_id}/img%03d.jpg -c:v libx264 -r 30 -pix_fmt yuv420p /tmp/#{message_id}/out.mp4`
    output = saveToS3(message_id)
    `rm -rf /tmp/#{message_id}`
    return output
  rescue
    return ""
  end
end

def performOperations()
  while @run do
      if @queue.length()>0
        msg = @queue.pop
        if(get_item(msg.message_id))
          output = createVideo(msg.body, msg.message_id)
          @output << "#{msg.message_id}\t#{output}\n"
          delete_item(msg.message_id)
        end
      end
  end
end

threads = Array.new

start_time = Time.now
threads << Thread.new{getMessagesFromSQS()}
sleep(2)
for i in 1..@noOfThreads
  threads << Thread.new{performOperations()}
end

threads.each { |thr| thr.join }

end_time = Time.now
puts "Time: #{TimeDifference.between(start_time, end_time).in_seconds} Sec"
File.open("output.txt", 'w') { |file| file.write(@output.join) }

