import java.net.*;
import java.io.*;

public class EchoClient
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
		BufferedReader in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
        PrintWriter out = new PrintWriter(clientSocket.getOutputStream(), true);
        
        // send a string to the server
        out.println("hello, it is me");
        out.flush();
        //out.close();
        
        // read a string back, should be all upcase
        String ret = in.readLine();
        System.out.println(ret);
	}
}
