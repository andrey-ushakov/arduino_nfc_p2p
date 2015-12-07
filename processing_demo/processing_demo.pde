import processing.serial.*;
import g4p_controls.*;

Serial arduinoPort;    // Create object from Serial class

GTextArea chatArea;
int chatH = 150;
GTextField msgField;
int msgFieldH = 20;

int sID = 0;
int friendIds[] = new int[5];
int friendsCnt = 0;
color c1         = color(255, 0, 0);
color c1_blink   = color(128, 0, 0);
color c2         = color(0, 255, 0);
color c2_blink   = color(0, 128, 0);
color c3         = color(0, 0, 255);
color c3_blink   = color(0, 0, 128);

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
}


void draw() {
  background(204);
  
  noStroke();
  textSize(32);
  
  //my planet
  fill(c1);
  ellipse(width/2, height/2-msgFieldH/2-chatH/2-5, 120, 120);
  text("Planet #"+sID, width/2-60, height/2-msgFieldH/2-chatH/2-5 + 100);
  
  
  
  
  
  for (int i = 0; i < friendsCnt; i++) {
    if(i == 0) {
      fill(c2);
      ellipse(120, 120, 90, 90);
      text("Planet #"+friendIds[i], 60, 200);
    } else if(i == 1) {
      fill(c3);
      ellipse(680, 120, 90, 90);
      text("Planet #"+friendIds[i], 620, 200);
    }
  }
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
  println("event !");
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
      println(cnt);
      
      //if(cnt == 1) {  // received own sID
      //  sID = int(inBuffer[0]);
      //} else {
        int friendID = int(inBuffer[0]);
        addFriend(friendID);
        String strMsg = new String(inBuffer, 0, cnt);
        String chatMsg = "He (#" + int(friendID) + ") :" + strMsg.substring(1);
        chatArea.appendText(chatMsg);
      //}
    }
  }
}




boolean addFriend(int id) {
  boolean flag = false;
  println(friendsCnt);
  for (int i = 0; i < friendsCnt; i++) {
    if(friendIds[i] == id) {
      flag = true;
      break;
    }
  }
  
  if(!flag) {
    println("add friend : " + id);
    friendIds[friendsCnt] = id;
    friendsCnt++;
  }
  
  return !flag;
}