#!/usr/bin/env ruby

# AWS CONFIG
require 'aws-sdk'
require 'time_difference'

Aws.config.update({
  region: 'us-west-2',
  credentials: Aws::Credentials.new('AKIAJFNYLH57Z6XUN6IQ', 'POnGJPjpFop/ZH0xRCx/qfVBknGvC9Cl2Ke/bmqR')
})

# CHECHING FOR ARGV
if(ARGV.length!=2)
  abort("Enter parameters corrently\nclient –s QNAME w <WORKLOAD_FILE>")
end
  
queuename = ARGV[0]
filename = ARGV[1]

sqs = Aws::SQS::Client.new
queue = sqs.create_queue({:queue_name=>queuename})

dynamoDB = Aws::DynamoDB::Client.new

start_time = Time.now

# Loop to push tasjk to SQS
File.open(filename, "r").each_line do |line|
  # Request task to SQS
  msg = sqs.send_message({:queue_url=>queue.queue_url, :message_body=>line.strip})
  # Saving message_id to dynamoDB to avoid duplication
  dynamoDB.put_item({
    table_name: "Messages", # required
    item: { # required
      "message_id" => msg.message_id, # value <Hash,Array,String,Numeric,Boolean,IO,Set,nil
    }
  })
end

end_time = Time.now
# Calculating time difference
puts "Time: #{TimeDifference.between(start_time, end_time).in_seconds} Sec"



