function handleInput(input){
  const id = input.id;
  const value = input.value;
  if(value.trim() === ""){
    document.getElementById(("history-" + id)).innerHTML = "";
    return;
  }

  let history = JSON.parse(localStorage.getItem(id) || "[]");
  if(!history.includes(value)){
    history.unshift(value);
    localStorage.setItem(id, JSON.stringify(history.slice(0, 5))); // Gioi han 5 lich su
  } 
}


function showHistory(input){
  const id = input.id;
  const history = JSON.parse(localStorage.getItem(id) || "[]"); 
  const list = document.getElementById("history-" + id);
  list.innerHTML = ""; // Xoa cu

  if(history.length === 0){
    list.style.display = "none"; //An neu khong co gi 
    return;
  }

  list.style.display = "block"; //Hien ra neu co du lieu

  history.forEach(item => {
    const li = document.createElement("li");
    li.textContent = item;
    li.onclick = () => {
      input.value = item;
      list.innerHTML = "";
      list.style.display = "none"; // An lai khi chon xong
    };
    list.appendChild(li);
  });
}


// An lich su sau khi click ra ngoai 
document.addEventListener("click", function(event){
  const historyLists = document.querySelectorAll(".history-list");

  historyLists.forEach(list => {
    const input = document.getElementById(list.id.replace("history-", ""));

    if(!list.contains(event.target) && event.target !== input){
      list.style.display = "none";
    }
  });
});