function startConnect(){
  return new Promise((resolve, reject) => {
    clientID = "clientID - " + parseInt(Math.random() * 100); //Tao so client ID ngau nhien

    //Lay gia tri tu cac truong input
    const host = document.getElementById("broker").value; /* ham cua JS de truy cap den phan tu HTML thong qua ID cua no */
    const port = document.getElementById("port").value;
    const userID = document.getElementById("username").value;
    const password = document.getElementById("password").value;

    //Hien thi thong tin ket noi len UI
    logMessage("Connecting to " + host + " on Port: " + port, true);
    logMessage("Client ID: " + clientID, true);

    const path = "/mqtt";

    //Tao Client MQTT
    client = new Paho.MQTT.Client(host, Number(port), path, clientID);

    //Gan cac callback fucntion
    client.onConnectionLost = onConnectionLost;
    client.onMessageArrived = onMessageArrived;

    //Cau hinh ket noi
    const options = {
      userName: userID,
      password: password,
      useSSL: true,
      onSuccess: () => { //Neu thanh cong
        resolve();
      },
      onFailure: (error) => { //Neu that bai
        reject(error);
      }
    };

    //Ket noi den broker
    client.connect(options);
  });
}


function onConnect(){
  logMessage("Connected to broker !", true);
  console.log("Connected to broker !");
}


function noConnect(error){
  logMessage("Failed to connect: " + error.errorMessage, true); //Hien thi len bang message
  console.error("Failed to connect: ", error); // Hien thi len bang console (f12)
  alert("Failed to connect to MQTT. Please try again ! "); //Canh bao len man hinh giao dien
}


function onConnectionLost(responseObject){
  logMessage("Connection Lost. Please try again !", true);
  if(responseObject !== 0){
    logMessage("Error: " + responseObject.errorMessage, true);
  }
}


function onMessageArrived(message){
  console.log("On message arrived: " + message.payloadString);
  logMessage("Topic <" + message.destinationName + "> | Message: " + message.payloadString);
}


function startDisconnect(){
  if(client && client.isConnected()){
    client.disconnect();
    logMessage("Disconnected from broker", true);
  }
}


function publishMessage(){
  const msg = document.getElementById("Message").value;
  const topic = document.getElementById("topic_p").value;

  if(!client || !client.isConnected()){
    alert("MQTT client is not connected !");
    return;
  }

  const message  = new Paho.MQTT.Message(msg);
  message.destinationName = topic;

  client.send(message);
  logMessage("Message: " + msg + " sent to topic <" + topic + ">");
}


function subscribeToTopic(){
  const topic = document.getElementById("topic_s").value;

  if(!client || !client.isConnected()){
    alert("MQTT client is not connected");
    return;
  }

  if(!topic){
    alert("Please enter a topic to subscribe");
    return;
  }

  client.subscribe(topic, {
    qos: 0,
    onSuccess: function() {
      logMessage("Subscribed to topic <" + topic + ">");
      console.log("Subscribed to topic <" + topic + ">");
      alert("Subscribed to topic <" + topic + ">");
    },
    onFailure: function(err) {
      logMessage("Failed to subscribe " + err.errorMessage, true);
      console.log("Failed to subscribe: ", err.errorMessage);
      alert("Failed to subscribe: " + err.errorMessage);
    }
  });
}


function logMessage(text, useHTML= false){
  const log = document.getElementById("messages");
  const span = document.createElement("span");

  if(useHTML){
    span.innerHTML = text; //Cho phep HTML (luu y ky tu <>)
  }else {
    span.textContent = text; //Tranh loi hien thi neu van ban co dau <> cua HTML 
  }

  log.appendChild(span);
  log.appendChild(document.createElement("br"));
  // log.innerHTML += "<span>" + text + "</span><br>";
}