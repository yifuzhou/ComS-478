import java.io.BufferedReader;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.PrintStream;
import java.io.PrintWriter;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.io.InputStream;  
import java.io.OutputStream; 

class Beacon {
	int ID;
	int StartUpTime;
	byte IP[];
	int CmdPort;
	Beacon(byte[] data) {
		this.ID = b2i(data, 0);
		this.StartUpTime = b2i(data, 4);
		this.IP = new byte[4];
		this.IP[0] = data[8];
		this.IP[1] = data[9];
		this.IP[2] = data[10];
		this.IP[3] = data[11];
		this.CmdPort = b2i(data, 12);
//		System.out.println("this is ID" + this.ID);
//		System.out.println("this is StartUpTime" + this.StartUpTime);
	}
	
	//transfer from byte to integer.
	int b2i(byte[] data, int offset) {
		return ((0xff & data[offset+3]) << 24)
				| ((0xff & data[offset+2]) << 16)
				| ((0xff & data[offset+1]) << 8)
				| (0xff & data[offset]);
	}
}

class GlobalValue{
	private GlobalValue(){};
	static ArrayList<Beacon> BeaconList = new ArrayList<Beacon>();
	static ArrayList<Integer> BeaconTimeRecorder = new ArrayList<Integer>();
	static int std_TimeRecorder = 0;
}

class AgentMonitor extends Thread {
	@Override
	public void run()
	{
		while (true)
		{
			try {
	            Thread.sleep(1000);
	            GlobalValue.std_TimeRecorder ++;
				//System.out.println(GlobalValue.std_TimeRecorder);
				
	        } catch (InterruptedException e) {
	            e.printStackTrace();
	        }
			for (int i =0; i< GlobalValue.BeaconTimeRecorder.size(); i++)
			{
				if (GlobalValue.std_TimeRecorder == GlobalValue.BeaconTimeRecorder.get(i) + 20)
					System.out.println("The agent whose ID: " + GlobalValue.BeaconList.get(i).ID +
							" is dead!(more than two beacons are missing) ");
			}
		}
		
	}
}

class BeaconListener extends Thread {
	
	public static ArrayList<Beacon> beaconList; 
	BeaconListener(byte[] data) 
	{
		Beacon beacon = new Beacon(data);
		int runCMD = isInList(beacon);
		if (runCMD == 2)
		{
			ClientAgent c = new ClientAgent(beacon);
			 c.start();
		}
	}
	
	/*
	 * The function checks beacon is in the list and return the value
	 * 2: new beacon
	 * 1: agent restarted(have same ID but different startuptime in the list) 
	 * 0: agent is dead(more than two beacons are missing)
	 * -1: recorded beacon
	 */
	int isInList(Beacon beacon)
	{
		for (int i =0; i< GlobalValue.BeaconList.size();i++)
		{
			if (GlobalValue.BeaconList.get(i).ID == beacon.ID&&
					GlobalValue.BeaconList.get(i).StartUpTime == beacon.StartUpTime)
			{
				GlobalValue.BeaconTimeRecorder.set(i, GlobalValue.BeaconTimeRecorder.get(i) + 10);
				return -1;
			}
				
			if (GlobalValue.BeaconList.get(i).ID == beacon.ID&&
					GlobalValue.BeaconList.get(i).StartUpTime != beacon.StartUpTime)
			{
				GlobalValue.BeaconList.set(i, beacon);
				GlobalValue.BeaconTimeRecorder.set(i, GlobalValue.std_TimeRecorder);
				System.out.println("Agent whose ID is " + beacon.ID + "has restarted.");
				return 1;
			}
				
		}
		GlobalValue.BeaconList.add(beacon);
		GlobalValue.BeaconTimeRecorder.add(GlobalValue.std_TimeRecorder);
		System.out.println("Success add into the list.");
		System.out.println("Received new beacon from agent's ID: " + beacon.ID);
		return 2;
	}
}

class ClientAgent extends Thread {
	boolean flag = true;

	ClientAgent(Beacon beacon)
	{
			System.out.println("Start to send command through TCP");
			//waiting for 1 second, for neat
			try {
	            Thread.sleep(1000);
	        } catch (InterruptedException e) {
	            e.printStackTrace();
	        }
			
			try {
	            //Create Socket object
	            Socket socket=new Socket("localhost",beacon.CmdPort);

	            InputStream inputStream=socket.getInputStream();
	            InputStreamReader inputStreamReader=new InputStreamReader(inputStream);//packaging
	            BufferedReader bufferedReader=new BufferedReader(inputStreamReader);//Buffer
	            
	            //information from client
	            String info = null;
	            if((info = bufferedReader.readLine())!=null){
	            	String [] s = info.split("#");
	                System.out.println("The OS of client is："+s[0]);
	                System.out.println("The StartUpTime of client is on ："+s[1]);
	            }
	            
	            //close 
	            bufferedReader.close();
	            inputStream.close();
	            socket.close();
	            
	        } catch (UnknownHostException e) {
	            e.printStackTrace();
	        } catch (IOException e) {
	            e.printStackTrace();
	        }
			
	}
	public void GetLocalOS()
	{
		
	}
}


public class Manager {
	static final int UDP_PORT = 4466;
	
	public static void main(String[] args) {

		System.out.println("Start running the manager!");
		
		AgentMonitor m = new AgentMonitor();
		 m.start();
		byte[] buffer = new byte[1024];
		DatagramPacket incoming = new DatagramPacket(buffer, buffer.length);
		DatagramSocket socket;
		try {
			socket = new DatagramSocket(UDP_PORT);
			System.out.println("Waiting beacons...");
			for (;;) {
				try {
					socket.receive(incoming);
					byte[] data = new byte[incoming.getLength()];
					System.arraycopy(incoming.getData(), 0, data, 0, data.length);

					// new thread to process beacon
					new BeaconListener(data).start();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		} catch (SocketException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}

}
}
