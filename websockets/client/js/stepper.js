$(document).ready(function() {
    registerWebsockets(function() {
        registerForStepperEvents();
    });
    registerButtonListeners();
});

function registerButtonListeners() {    
    $("#forward360").on("click", function() {
        forward(true);
    });
    
    $("#backward360").on("click", function() {
        backward(false);
    }); 
}

function onStepperEvent(topicUri, event) {
    console.log(event);
}

function registerForStepperEvents() {
    sess.subscribe("/stepper#forward", onStepperEvent);
    sess.subscribe("/stepper#backward", onStepperEvent);
    sess.subscribe("/stepper#stop", onStepperEvent);
}
    
function forward() {
   sess.call("/stepper#forward").then(ab.log, ab.log);
}

function backward() {
   sess.call("/stepper#backward").then(ab.log, ab.log);
}