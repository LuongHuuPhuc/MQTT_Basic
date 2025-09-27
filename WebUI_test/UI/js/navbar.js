function showPage(pageId, link){
  document.querySelectorAll('.page').forEach(p => p.classList.remove('active'));
  document.querySelectorAll('.navbar a').forEach(a => a.classList.remove('active'));

  document.getElementById(pageId).classList.add('active');
  link.classList.add('active');
}