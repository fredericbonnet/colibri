```html
<script src="viz.js"></script>
```

```js
mermaid.initialize({
  // theme: "base",
  themeVariables: {
    // primaryColor: "#2C6FE0",
    // textColor: "black",
    // primaryTextColor: "#2C6FE0",
    // defaultLinkColor: "#2C6FE0",
    // darkMode: true,
    // fontSize: "14px",
    // fontFamily: "sans-serif",
  },
});

let dotId = 0;
function renderDot(code, link) {
  // Generate Graphviz Dot SVG into container
  const div = document.createElement("div");
  div.className = "dot";
  const id = "dot-svg-" + dotId++;
  div.id = id;

  const worker = new Worker(
    window.$docsify.dot ? window.$docsify.dot.worker : "viz.render.js"
  );
  worker.onmessage = (e) => {
    const div = document.getElementById(id);
    div.innerHTML = e.data.result;
    const div2 = document.createElement("div");
    div.querySelectorAll("a").forEach((a) => {
      div2.innerHTML = link(a.getAttribute("xlink:href"));
      a.setAttribute("xlink:href", div2.firstChild.getAttribute("href"));
    });
    worker.terminate();
  };
  worker.onerror = (error) => {
    console.error(e);
    worker.terminate();
    throw error;
  };

  const params = {
    src: code,
    id: new Date().toJSON(),
    options: {
      files: [],
      format: "svg",
      engine: "dot",
    },
  };
  worker.postMessage(params);
  return div.outerHTML;
}

window.$docsify = {
// ...
  markdown: {
    renderer: {
      code: function (code, lang) {
        try {
          if (lang === "dot") {
            return renderDot(code, this.origin.link);
          }
// ...
```
