import processing.serial.*;
import g4p_controls.*;

Serial arduinoPort;    // Create object from Serial class

GTextArea chatArea;
int chatH = 150;
GTextField msgField;
int msgFieldH = 20;
//GButton sendBtn;

void setup() {
  size(800, 600);
  
  chatArea = new GTextArea(this, 0, height-chatH-msgFieldH-10, width, chatH, G4P.SCROLLBARS_BOTH | G4P.SCROLLBARS_AUTOHIDE);
  chatArea.setText(" ", 310);
  msgField = new GTextField(this, 0, height-msgFieldH-5, width, msgFieldH);
  msgField.tag = "msgField";
  
  String portName = Serial.list()[1]; //change the 0 to a 1 or 2 etc. to match your port
  println(portName);
  arduinoPort = new Serial(this, portName, 9600);
  arduinoPort.bufferUntil('\n');
  
  //sendBtn = new GButton(this, 0, 0, 120, 30);
  //sendBtn.setText("Send!");
}


void draw() {
  background(204);
}


public void displayEvent(String name, GEvent event) {
  switch(event) {
  case ENTERED:
    String msg = msgField.getText();
    arduinoPort.write(msg);
    chatArea.appendText("Me: " + msg + "\n");
    msgField.setText("");
    break;
  }
}

public void handleTextEvents(GEditableTextControl textControl, GEvent event) {
  if(textControl.tag == "msgField") {
    displayEvent(textControl.tag, event);
  }
}

/*
void handleButtonEvents(GButton button, GEvent event) {
  String msg = msgField.getText();
  println(msg);
  chatArea.appendText(msg + "\n");
  msgField.setText("");
}*/


void serialEvent (Serial myPort) {
  
  byte[] inBuffer = new byte[128];
  while (myPort.available() > 0) {
    //inBuffer = myPort.readBytes();
    myPort.readBytes(inBuffer);
    if (inBuffer != null) {
      int cnt = 0;
      for(int i=0; i<128; i++) {
        if(int(inBuffer[i]) != 13) {
          cnt++;
        } else {
          break;
        }
      }
      
      int sID = int(inBuffer[0]);
      String strMsg = new String(inBuffer, 0, cnt);
      String chatMsg = "He (" + int(sID) + ") :" + strMsg.substring(1);
      chatArea.appendText(chatMsg);
    }
  }
}