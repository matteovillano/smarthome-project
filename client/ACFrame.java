import javax.swing.*;
import java.awt.*; 
import java.io.*;
import java.net.*;
import java.util.*;
import java.lang.*;

class ACFrame extends JFrame{
	
	private ACListener list;
	private String ip;
	private int port;
	private Socket sock;
	private PrintWriter sockOut;
	private BufferedReader sockIn;
	
	
	
	//pan
	private final JPanel northPan=new JPanel();
	private final JPanel centerPan=new JPanel();
	private final JPanel southPan=new JPanel();
	
	//north panel
	private final JLabel ipLab=new JLabel("IP Address:");
	private final JLabel portLab=new JLabel("Port:");
	private final JTextArea ipTA=new JTextArea(1,10);
	private final JTextArea portTA=new JTextArea(1,10);
	private final JButton connectBtn=new JButton("Connect");
	private final JButton disconnectBtn=new JButton("Disconnect");
	
	//center panel
	private final JTextArea centerTA=new JTextArea(20,60);
	private final JScrollPane centerSP=new JScrollPane(centerTA,JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED,JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);
	
	//south panel
	private final JLabel mesLab=new JLabel("Messsage:");
	private final JTextField sTF=new JTextField(30);
	private final JButton sendBtn=new JButton("Send");
	
	//
	private boolean connected=false;
	
	public ACFrame(){
		super("Arduino smarthome client");
		
		//setting btns
		list=new ACListener(this);
		connectBtn.addActionListener(list);
		disconnectBtn.addActionListener(list);
		sendBtn.addActionListener(list);
		sTF.addActionListener(list);
		
		connectBtn.setActionCommand("1");
		disconnectBtn.setActionCommand("2");
		sendBtn.setActionCommand("3");
		sTF.setActionCommand("3");
		
		this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		//this.setSize(800,600);
		Container cont=this.getContentPane();
		
		//north
		northPan.setLayout(new GridLayout(1,6));
		northPan.add(ipLab);
		northPan.add(ipTA);
		northPan.add(portLab);
		northPan.add(portTA);
		northPan.add(connectBtn);
		northPan.add(disconnectBtn);
		
		
		//center
		centerTA.setEditable(false);
		centerPan.setSize(600, 400);
		centerPan.add(centerSP);
		
		//south
		southPan.add(mesLab);
		southPan.add(sTF,BorderLayout.CENTER);
		southPan.add(sendBtn,BorderLayout.WEST);
		
		cont.add(northPan,BorderLayout.NORTH);
		cont.add(centerPan,BorderLayout.CENTER);
		cont.add(southPan,BorderLayout.SOUTH);
		
		ip="127.0.0.1";
		port=8080;
		
		ipTA.setText(ip);
		portTA.setText(Integer.toString(port));
		
		setBtns();
		
		this.pack();
		this.setVisible(true);
	}
	
	
	//btns method
	public void connect() {
		//System.out.println("premuto connect");
		ip=ipTA.getText();
		port=Integer.parseInt(portTA.getText());
		try{
			sock=new Socket(ip,port);
			
			sockOut=new PrintWriter(sock.getOutputStream());
			sockIn=new BufferedReader(new InputStreamReader(sock.getInputStream()));
			connected=true;
			appendText("Connected...");
		}catch(Exception e){appendText(e.getMessage());}
		
		setBtns();
	}
	
	public void disconnect() {
		//System.out.println("premuto disconnect");
		try{
			sock.close();
			connected=false;
			appendText("Disconnected...");
		}catch(Exception e){appendText(e.getMessage());}
		setBtns();
	}
	
	public void send() {
		if(!connected)return;
		try{
			String s=sTF.getText();
			sockOut.println(s);
			sockOut.flush();
			sTF.setText("");
			appendText("sent:\n "+s);
			appendText("recived: ");
			String r;
			char[] buff=new char[256];
			int i=0;
			while(true){
				char r1=(char)sockIn.read();
				if(r1=='\n'&&buff[i-1]=='\r')break;
				buff[i]=r1;
				i++;
			}
			r=new String(buff);
			appendText(r);
		}catch(Exception e){
			appendText(e.getMessage());
			disconnect();
		}	
	}
	
	//aux methods
	private void setBtns(){
		if(!connected) {
			connectBtn.setEnabled(true);
			disconnectBtn.setEnabled(!true);
			sendBtn.setEnabled(!true);
			sTF.setEditable(!true);
			ipTA.setEditable(true);
			portTA.setEditable(true);
		}else {
			connectBtn.setEnabled(!true);
			disconnectBtn.setEnabled(true);
			sendBtn.setEnabled(true);
			sTF.setEditable(true);
			ipTA.setEditable(!true);
			portTA.setEditable(!true);
		}
		
		
	}
	
	private void appendText(String s){
		centerTA.setText(centerTA.getText()+"\n"+s);
	}
}
