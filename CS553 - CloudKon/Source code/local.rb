#!/usr/bin/env ruby

require 'time_difference'

@queue = Queue.new
@output = Array.new


# Reading file and Pushing task to in memory queue
def readTasksFromFile(filename)
  File.open(filename, "r") do |f|
    f.each_line do |line|
      @queue << line
    end
  end
end


# Executing task 
def performOperations()
  while @queue.length()!=0 do
    begin
      msg = @queue.pop.strip
      output = `#{msg}`
      @output << "#{msg}\t#{output}\n"
    rescue
      puts "Error"
    end
  end
end

noOfThreads = ARGV[0].to_i
threads = Array.new

start_time = Time.now

readTasksFromFile(ARGV[1])

for i in 1..noOfThreads
  threads << Thread.new{performOperations()}
end

threads.each { |thr| thr.join }

end_time = Time.now
puts "Time: #{TimeDifference.between(start_time, end_time).in_seconds} Sec"

File.open("output.txt", 'w') { |file| file.write(@output.join) }

