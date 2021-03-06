$(document).ready(function() {
    registerWebsockets(function() {
        registerForLcdEvents();
        getCurrentBacklightState();
    });
    registerButtonListeners();
});

function registerButtonListeners() {
    $("#write").on("click", writeBothRowsToLcd);
    
    $("#clear").on("click", function() {
        $("#textForLcdRowOne").val("");
        $("#textForLcdRowTwo").val("");
        clearLcd();
    });
    
    $("#backlightOn").on("click", function() {
        backlight(true);
    });
    
    $("#backlightOff").on("click", function() {
        backlight(false);
    }); 
}

function onLcdBacklightEvent(topicUri, event) {
    showCurrentBacklightState(event);
}

function registerForLcdEvents() {
    sess.subscribe("/lcd#backlight", onLcdBacklightEvent);
}

function getCurrentBacklightState() {
    sess.call("/lcd#backlightState").then(showCurrentBacklightState, ab.log);
}

function showCurrentLcdText(text) {
    $("#textInLcdRowOne").text(text);
}

function showCurrentBacklightState(state) {
    if(state) {
        $("#backlightOn").removeClass("btn-default");
        $("#backlightOn").addClass("btn-primary");
        $("#backlightOff").removeClass("btn-primary");
        $("#backlightOff").addClass("btn-default");
    } else {
        $("#backlightOn").removeClass("btn-primary");
        $("#backlightOn").addClass("btn-default");
        $("#backlightOff").removeClass("btn-default");
        $("#backlightOff").addClass("btn-primary");
    }
}

function writeBothRowsToLcd() {
    var rowOneValue = $("#textForLcdRowOne").val();
    setLcdRow(0);
    writeToLcd(rowOneValue);
    var rowTwoValue = $("#textForLcdRowTwo").val();
    setLcdRow(1);
    writeToLcd(rowTwoValue);
}
    
function writeToLcd(text) {
   sess.call("/lcd#write", text).then(ab.log, ab.log);
}

function readFromLcd() {
   sess.call("/lcd#read").then(ab.log, ab.log);
}

function clearLcd() {
    sess.call("/lcd#clear").then(ab.log, ab.log);
}

function setLcdRow(row) {
    sess.call("/lcd#row", row).then(ab.log, ab.log);
}

function backlight(on) {
    sess.call("/lcd#backlight", on).then(ab.log, ab.log);
}