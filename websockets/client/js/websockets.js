var sess = null;

function registerWebsockets(callback) {
   var wsuri = "ws://" + window.location.hostname + ":9000";
   ab.connect(wsuri,
      function (session) {
         sess = session;
         console.log("Connected to " + wsuri + "!");
         $("#connected").text("Connected");
         callback();
      },
      function (code, reason) {
         sess = null;
         $("#connected").text("Disconnected");
      }
   );
};