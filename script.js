function handleSizeChange() {
  const video = document.querySelector("video");

  const windowAspectRatio = window.innerWidth / window.innerHeight;
  const videoAspectRatio = video.clientWidth / video.clientHeight;

  const landscapeScale = videoAspectRatio / windowAspectRatio;
  const portraitScale = windowAspectRatio / videoAspectRatio;

  var scale = landscapeScale >= 1 ? landscapeScale : portraitScale;
  scale = Math.ceil(scale * 10) / 10;

  console.log(window.innerWidth);
  console.log(window.innerHeight);
  document.body.style.transform = `scale(${scale})`;
  document.body.style.filter = `saturate(1.5) blur(60px)`;
}

function main() {
  const video = document.querySelector("video");
  if (!video) {
    setTimeout(main, 10);
    return;
  }

  new ResizeObserver(handleSizeChange).observe(video);
  window.addEventListener("resize", handleSizeChange);
  handleSizeChange();
}

document.addEventListener("DOMContentLoaded", main);
