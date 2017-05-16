import java.io.IOException;
import java.util.StringTokenizer;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;

public class HadoopSort {
    
    public static class TokenizerMapper extends Mapper<Object, Text, Text, Text>{
        
        private Text word1 = new Text();
        private Text word2 = new Text();
        
        public void map(Object key, Text value, Context context) throws IOException, InterruptedException {
            String str= value.toString();
            word1.set(str.substring(1, 10));
            word2.set(str.substring(12) + "\r\n");
            context.write(word1, word2);
        }
    }
    
    public static class IntSumReducer extends Reducer<Text,Text,Text,Text> {
        
        public void reduce(Text key, Text value, Context context) throws IOException, InterruptedException {
            
            context.write(key, value);
        }
    }
    
    public static void main(String[] args) throws Exception {
        Configuration conf = new Configuration();
        conf.set("mapreduce.output.textoutputformat.separator","  ");
        
        Job job = Job.getInstance(conf, "Sort");
        job.setJarByClass(HadoopSort.class);
        job.setMapperClass(TokenizerMapper.class);
        job.setReducerClass(IntSumReducer.class);
        job.setOutputKeyClass(Text.class);
        job.setOutputValueClass(Text.class);
        FileInputFormat.addInputPath(job, new Path(args[0]));
        FileOutputFormat.setOutputPath(job, new Path(args[1]));
        System.exit(job.waitForCompletion(true) ? 0 : 1);
    }
}