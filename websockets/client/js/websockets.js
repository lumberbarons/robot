var sess = null;

function registerWebsockets(callback) {
   var wsuri = "ws://" + window.location.hostname + ":9000";
   ab.connect(wsuri,
      function (session) {
         sess = session;
         console.log("Connected to " + wsuri + "!");
         callback();
      },
      function (code, reason) {
         sess = null;
         alert(reason);
      }
   );
};