var express = require('express');
var path = require('path');
var favicon = require('serve-favicon');
var logger = require('morgan');
var cookieParser = require('cookie-parser');
var bodyParser = require('body-parser');
var serialPort = require('serialport');
var url = require('url');
var baud = 9600;
var db = require('mongodb');
var comms = listSerial();
var comms = "/dev/cu.usbmodem1421";
//var comms2 = "/dev/cu.usbmodem1411";


var routes = require('./routes/index');
var users = require('./routes/users');


var app = express();
var message = "num:";
var counter = 10;
var clickCount;


/*
*  Init the base express
*/
var sp = openSP(comms, baud);
var rData = {};
var colors = {};

listSerial();

sp.open(function (error){
  if (error) {
    console.log('failed to open ->: '+error);
  }
  else {
    sp.on('data', function(data) {
      console.log("we have a data response", data.toString());
      parseSerialData(data);
    });
  }
});

function openSP(port, baud){
  var serial = serialPort.SerialPort;

  return new serial(port, {
    baudrate: baud
  }, false);
}

app.get('/data', function(request, response) {
  response.json(rData);
});

app.get('/myo', function(req, res){
  var url_parts = url.parse(req.url, true);
  var query = url_parts.query;
  console.log(query.pose);

  //Extract the number included in the query

  sp.write(query.pose + "\r", function(err, results) {
      if (err) {
          console.log (err);
      }
      else {
          console.log(results);
      }
  });
  // console.log(query);
  res.sendStatus(200);

});

app.get('/frogger', function(req, res){
  //"color1 = r, color2 = g, color3 = b, brightness"
  //Set defaults.
  var url_parts = url.parse(req.url, true);
  var query = url_parts.query;

  console.log(query.color1);
  console.log(query.color2);
  console.log(query.color3);
  console.log(query.brightness);

  var message = string.concat(query.color1.toString(),",", query.color2.toString(),",", query.color3.toString());
  console.log(message);

  if(sp.isOpen()){
    sp.write(message + "\r", function(err, results) {
      if(err) {
        console.log("fail ->: " +err);
      } else {
        console.log(results);
      }
    });
  }
});

//app.get('/', routes.index);
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: false }));

app.post('/click', function(req, res){
  console.log("Light Up this bitch");
  //Send a buffered signal to the serialPort
  //sp write

  message = counter.toString();
  counter+=1;
  console.log("Writing serial data: "+ message);
  if(sp.isOpen()){
    if(counter%2 === 0){
      clickCount = 1;
      sp.write(clickCount.toString() + "\r", function(err, results) {
        console.log(clickCount);
        if(err) {
          console.log("fail ->:" +err);
        } else {
          console.log(results);
        }
      });
    }else{
      clickCount = 0;
      sp.write(clickCount.toString() + "\r", function(err, results) {
        console.log(clickCount);
        if(err) {
          console.log("fail ->:" +err);
        } else {
          console.log(results);
        }
      });
    }
    // sp.write(clickCount + "\r", function(err, results) {
    //   console.log(clickCount);
    //   if(err) {
    //     console.log("fail->:" +err);
    //   }
    //   else {
    //     console.log(results);
    //     //AJAX call to second page
    //   }
    // });
  }
  res.render('index.jade');
});

app.post('/offClick', function(req, res){
  console.log("Turn this bitch off");
  //Send a buffered signal to the serialPort
  //sp write

  message = counter.toString();
  counter+=1;

  console.log("Writing serial data: "+ message);
  if(sp.isOpen()){
    sp.write(clickCount + "\r", function(err, results) {
      if(err) {
        console.log("fail->:" +err);
      }
      else {
        console.log(results);
        //AJAX call to second page
      }
    });
  }
  res.render('index.jade');
});

// app.get('/', function(request, response) {
//   console.log("We have data call", request.path);
//   response.render('index.jade', {title})
// });

function listSerial(){
  var usbComm;
  serialPort.list(function(err, ports) {
    ports.forEach(function(port){
      if (port.comName.indexOf("blue") != 1){
        console.log("usb", port.comName);
        usbComm = port.comName;
        return port.comName;
      }
    });
    return usbComm;
  });
}

function parseSerialData(data) {
  data = (data.toString().trim());

  var tempArray = new Array();

  if(data.length == 0) {
    return;
  }

  var dof = /Temperature|Humidity|Wind/i;
  if (dof.test(data)) {
    tempArray = data.split(/:/);

    rData[tempArray[0]] = tempArray[1];
    return;
  }
  var radar = /radar/i;
  if(radar.test(radar)) {
    console.log("radar data");
    return;
  }
}
// view engine setup
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'jade');

// uncomment after placing your favicon in /public
//app.use(favicon(path.join(__dirname, 'public', 'favicon.ico')));
app.use(logger('dev'));
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: false }));
app.use(cookieParser());
app.use(express.static(path.join(__dirname, 'public')));

app.use('/', routes);
app.use('/users', users);

// catch 404 and forward to error handler
app.use(function(req, res, next) {
  var err = new Error('Not Found');
  err.status = 404;
  next(err);
});

// error handlers

// development error handler
// will print stacktrace
if (app.get('env') === 'development') {
  app.use(function(err, req, res, next) {
    res.status(err.status || 500);
    res.render('error', {
      message: err.message,
      error: err
    });
  });
}

// production error handler
// no stacktraces leaked to user
app.use(function(err, req, res, next) {
  res.status(err.status || 500);
  res.render('error', {
    message: err.message,
    error: {}
  });
});


module.exports = app;
