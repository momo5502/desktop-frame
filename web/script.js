function handleSizeChange() {
  const video = document.querySelector("video");

  const windowAspectRatio = window.innerWidth / window.innerHeight;
  const videoAspectRatio = video.clientWidth / video.clientHeight;

  const landscapeScale = videoAspectRatio / windowAspectRatio;
  const portraitScale = windowAspectRatio / videoAspectRatio;

  var scale = landscapeScale >= 1 ? landscapeScale : portraitScale;
  scale = Math.ceil(scale * 10) / 10;
  scale += 0.1;

  console.log(window.innerWidth);
  console.log(window.innerHeight);
  document.body.style.transform = `scale(${scale})`;
 // document.body.style.filter = `saturate(1.5) blur(80px)`;
}

function injectStyles() {
  const styles = document.createElement("style");
  styles.textContent = `

  .momo-widget,
  .momo-widget * {
    color: #222;
    font-family: "Segoe UI", Tahoma, Geneva, Verdana, sans-serif;
  }

.momo-widget {
    z-index: 9999;
    position: fixed;
    margin: 20px;
    padding: 30px;
    backdrop-filter: blur(30px) saturate(2);
    box-shadow: 0 0 30px 0 rgba(0, 0, 0, 0.25);
    border-radius: 15px;
    background-color: rgba(255, 255, 255, 0.3);

    right: 50px;
    bottom: 60%;
    min-width: 50px;
    min-height: 50px;
    font-size: 20px;

    min-height: 200px;
}

.momo-widget-2 {
  bottom: 30%;
}
  `;

  const htmlContent = `
    <span style="font-size: 35px">Sample Widget</span>
    <p>
      Something interesting 😀
    </p>
  `;

  const htmlContent2 = `
  <span style="font-size: 35px">Sample Widget</span>
  <p>
    Nothing interesting 🫥
  </p>
`;

  const widget = document.createElement("div");
  widget.classList.add("momo-widget");
  widget.innerHTML = htmlContent;

  const widget2 = document.createElement("div");
  widget2.classList.add("momo-widget");
  widget2.classList.add("momo-widget-2");
  widget2.innerHTML = htmlContent2;

  document.body.appendChild(styles);
  document.body.parentElement.appendChild(widget);
  document.body.parentElement.appendChild(widget2);
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

  //injectStyles();
}

document.addEventListener("DOMContentLoaded", main);
