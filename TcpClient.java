import java.net.*;
import java.io.*;

public class TcpClient
{
	public static void main(String args[]) throws IOException
	{
		String serverIP = "127.0.0.1";
		
		if (args.length != 0)
		{
			serverIP = args[0]; // the ip from user input
		}
		
		// create a socket, which immediately connect to the server
		Socket clientSocket = new Socket(serverIP, 1234); 

		// create text reader and writer
		DataInputStream inStream  = new DataInputStream(clientSocket.getInputStream());
		DataOutputStream outStream = new DataOutputStream(clientSocket.getOutputStream());

		// prepare a binary buffer
		String s = "this is a test";
		byte[] buf = s.getBytes();
		
		// send the length, and then buffer
		byte[] bufLengthInBinary = toBytes(buf.length);
		
		printBinaryArray(bufLengthInBinary, "here");
		
		// send 4 bytes
		outStream.write(bufLengthInBinary, 0, bufLengthInBinary.length);
		// send the string
		outStream.write(buf, 0, buf.length);
		outStream.flush();
		
		// read the data back
        inStream.readFully(bufLengthInBinary); // ignore the first 4 bytes
        inStream.readFully(buf); // 
     
        // convert the binary bytes to string
        String ret = new String(buf);
        
        // should be all upcases now
        System.out.println(ret);
	}
	
	static void printBinaryArray(byte[] b, String comment)
	{
		System.out.println(comment);
		for (int i=0; i<b.length; i++)
		{
			System.out.print(b[i] + " ");
		}
		System.out.println();
	}

	static private byte[] toBytes(int i)
	{
	  byte[] result = new byte[4];

	  result[0] = (byte) (i >> 24);
	  result[1] = (byte) (i >> 16);
	  result[2] = (byte) (i >> 8);
	  result[3] = (byte) (i /*>> 0*/);

	  return result;
	}
}
