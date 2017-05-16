import java.awt.Color;
import java.awt.image.BufferedImage;
import java.io.File;
import java.util.ArrayList;

import javax.imageio.ImageIO;

import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;

/**
 * @author Kunal Kolhatkar
 * 		   Kunal Dhande
 * 
 * @version 1.0
 */

public class SmearDetector {
	
	/**
	 * @name: 		 Main()
	 * @param args:  Takes the path as user input in form of command line argument
	 * 				 to get the list of all the files in folder in the provided path
	 */
	public static void main(String[] args) {

		String workingDir = "D:\\sample_drive\\cam_3\\"; 		// Setting the path of folder from user
		File folder = new File(workingDir);
		File[] listOfFiles = folder.listFiles();
		ArrayList list = new ArrayList();
														// 	Creating list of all the files in folder and checking all selecting
		for (int i = 0; i < listOfFiles.length; i++) { 	//	 all those with .jpg format
			if (listOfFiles[i].isFile()) {
				if (listOfFiles[i].getName().endsWith(".jpg")) {
					list.add(listOfFiles[i].getName());
				}
			} else if (listOfFiles[i].isDirectory()) {
			}
		}
		int size = list.size();
		
		SmearDetector.AveragingPics(size, list, workingDir);
	}
	
	
	
	/**
	 * @name : AveragingPics(int size, ArrayList list, String workingDir)	
	 * 		   Calculates the average of all the pictures in current folder
	 * @param size : Size of List(Number of files in folder)
	 * @param list : List of files in folder
	 * @param workingDir : The working directory
	 */
	public static void AveragingPics(int size, ArrayList list, String workingDir) {
		try {

			BufferedImage image2 = ImageIO.read(new File(workingDir + list.get(0)));
			int height = image2.getHeight();	// gets Height of photo
			int width = image2.getHeight();		// gets Width of photo
			int totalPixel = height * width;    // Calculating total number of pixel in photo
			
			// Creating an array of Red, Green, Blue to store respective pixel values from of all the pictures
			int red[] = new int[totalPixel];	 
			int green[] = new int[totalPixel];	
			int blue[] = new int[totalPixel];	

			BufferedImage image;
			for (int i = 0; i < size; i++) {
				image = ImageIO.read(new File(workingDir + list.get(i)));

				int pixelNumber = 0;	//Counter to store Pixel number		
				for (int j = 0; j < height; j++) {
					for (int k = 0; k < width; k++) {
						Color c = new Color(image.getRGB(k, j));  //Getting RGB value of current pixel
						red[pixelNumber] += c.getRed();			  //Adding respective values to array
						blue[pixelNumber] += c.getBlue();
						green[pixelNumber] += c.getGreen();
						
						// When its last image calculating average by dividing by total number of images in folder
						if (i == size - 1) {
							red[pixelNumber] = red[pixelNumber] / size; 
							green[pixelNumber] = green[pixelNumber] / size;
							blue[pixelNumber] = blue[pixelNumber] / size;
							Color newColor = new Color(red[pixelNumber], green[pixelNumber], blue[pixelNumber]);
							image2.setRGB(k, j, newColor.getRGB());
						}
						pixelNumber++;
					}
				}
			}

			File ouptut = new File(workingDir + "Average.jpg");	//Providing the path where the Averaged Image is to be created
			ImageIO.write(image2, "jpg", ouptut);					//Writing the file to disk
			SmearDetector.SmearDetection(workingDir);

		} catch (Exception e) {
			System.err.println(e);
		}
	}
	


	/**
	 * @name: SmearDetection(): Takes the averaged Picture as Input and detects the smear
	 * 		  and generates a final image 
	 * @param workingDir  : Name of Averaged file
	 */
	public static void SmearDetection(String workingDir) {
		try {
			
			String temp = workingDir+"Average.jpg";
			System.loadLibrary(Core.NATIVE_LIBRARY_NAME);	// Loads the openCV library in memory
			Mat original = Imgcodecs.imread(temp, Imgcodecs.CV_LOAD_IMAGE_COLOR);	
			Mat source = Imgcodecs.imread(temp, Imgcodecs.CV_LOAD_IMAGE_GRAYSCALE);	// Creates Gray scale image of averaged image 
			Mat destination = new Mat(source.rows(), source.cols(), source.type());

			Imgproc.adaptiveThreshold(source, destination, 255, Imgproc.ADAPTIVE_THRESH_GAUSSIAN_C,
					Imgproc.THRESH_BINARY, 275, 10);				//Adaptive thresholding to average image and scale to 275 

			Imgproc.blur(destination, destination, new Size(3, 3));
			Imgproc.Canny(destination, source, 155.0, 200.0);		//Detect boundary of selected area in the images 

			String temp2 = workingDir+"Intermediatery_file.jpg";
			Imgcodecs.imwrite(temp2, destination);

			BufferedImage image = ImageIO.read(new File(temp2));
			int height = image.getHeight();
			int width = image.getHeight();

			int Xmin = width + 1;
			int Xmax = 0;
			int Ymin = height + 1;
			int Ymax = 0;

			for (int j = 0; j < height; j++) {   	//Getting the coordinates of smear need to create rectangle
				for (int k = 0; k < width; k++) {
					Color c = new Color(image.getRGB(k, j));
					if (c.getRed() + c.getBlue() + c.getGreen() == 0) {
						if (k < Xmin) {
							Xmin = k;
						}
						if (k > Xmax) {
							Xmax = k;
						}
						if (j < Ymin) {
							Ymin = j;
						}
						if (j > Ymax) {
							Ymax = j;
						}
					}
				}
			}
													// Create rectangle with the given points
			Imgproc.rectangle(original, new Point(Xmin, Ymin), new Point(Xmax, Ymax), new Scalar(0, 0, 255), 3);
			workingDir = workingDir+"Final.jpg";
			Imgcodecs.imwrite(workingDir, original);		// Writing final image to file 

			System.out.println("file Created");

		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}
