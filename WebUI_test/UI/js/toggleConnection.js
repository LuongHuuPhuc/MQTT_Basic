let isConnected = false;

async function toggleConnection(){
  const btn = document.getElementById("connect-btn");

  if(!isConnected){
    //Doi sang trang thai connected
    try{
      await startConnect(); //Cho ket noi xong moi doi trang thai
      btn.value = "Disconnect";
      btn.classList.add("connected");
      onConnect(); //Thong bao da ket noi thanh cong
      isConnected = true;
    } 
    
    catch(error){
      noConnect(error); //Thong bao ket noi that bai
      isConnected = false;
    }

  }else {
    startDisconnect();  //Ham nay co the la synchronous hoac async 
    btn.value = "Connect"; 
    btn.classList.remove("connected");
    isConnected = false;
  }
}