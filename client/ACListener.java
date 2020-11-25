import javax.swing.event.*;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

public class ACListener implements ActionListener{

	private final ACFrame frame;
	
	public ACListener(ACFrame f) {
		frame=f;
		
	}
	
	
	public void actionPerformed(ActionEvent e) {
		String s=e.getActionCommand();
		
		switch(s) {
			case "1":
				frame.connect();
				break;
			case "2":
				frame.disconnect();
				break;
			case "3":
				frame.send();
				break;
			case "4":
				frame.cleanTA();
				break;
		}
		
	}
	
}
