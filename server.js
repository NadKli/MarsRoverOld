//...
const SerialPort = require('serialport');
const Readline = require('@serialport/parser-readline');
var express=require('express');
var app=express();
var http=require('http').Server(app);
var io=require('socket.io')(http);
const router=express.Router();
const path=require('path');
const { lstatSync } = require('fs');
const port = new SerialPort('COM4', { baudRate: 9600 });
const parser = port.pipe(new Readline({ delimiter: '\n' }));

app.use(express.static(__dirname+'/SiteTest'));

app.get('/home',function(req,res){
    res.sendFile(path.join(__dirname+'/SiteTest/Home.html'));
});
app.get('/stream',function(req,res){
    res.sendFile(path.join(__dirname+'/SiteTest/Live-Feed.html'));
});
app.get('/contact',function(req,res){
    res.sendFile(path.join(__dirname+'/SiteTest/Contact.html'));
});


//Listen in the serial for messages from the arduino
var DataRec=0;

port.on("open", () => {
    console.log('Serial port open');
  });
parser.on('data', data =>{
    console.log('Got word from arduino:', data);
    DataRec = data.split(',');   //split data and put them into variables

    var Distance = DataRec[0];
    var Temperature = DataRec[1];
    var Humidity = DataRec[2];
    var Pitch = DataRec[3];
    var Roll = DataRec[4];
    var Yaw = DataRec[5];
   

    io.sockets.on('connection',function(socket){
        io.sockets.emit('Pitch',{Pitch:Pitch, Roll:Roll, Yaw:Yaw});
        
        io.sockets.emit('Data Pannel',{Distance:Distance, Temperature:Temperature, Humidity:Humidity});
    });
  });


// var Pitch = "27Z02145";
// var Roll = "F47876";

var lat =32.2181727;
var lon=-7.9356577;

var SpeedValue=0;
var AngleValue=0;
var Sens=1;

io.sockets.on('connection',function(socket){
        io.sockets.emit('Coord',{lat:lat, lon:lon});
        socket.on('SpeedValue',function(data){
            SpeedValue=data;
            console.log("SpeedValue : ", SpeedValue);
            if(SpeedValue>0){
                Sens=1;
                DataSend = Sens+','+SpeedValue+','+AngleValue+'\n';
            }
            else if(SpeedValue<0){
                Sens=0;
                SpeedValue=SpeedValue*(-1);
                DataSend = Sens+','+ SpeedValue+','+AngleValue+'\n';
                console.log(DataSend);
            }
           
            console.log("Data Send : ",DataSend)

//             //Write in the serial for the arduino

            port.write(DataSend, (err) => {
                if (err) {
                  return console.log('Error on write: ', err.message);
                }
                console.log('message written to Arduino');
              });
        });
        socket.on('AngleValue',function(data){
            AngleValue=data;
            console.log("AngleValue : ", AngleValue);
            DataSend = Sens+','+SpeedValue+','+AngleValue+'\n'
            console.log("Data Send : ",DataSend)

            //Write in the serial for the arduino (Vitesse,Angle)
            port.write(DataSend, (err) => {
                if (err) {
                  return console.log('Error on write: ', err.message);
                }
                console.log('message written to Arduino');
              });
        });
        
});

http.listen(1234,function(){
    console.log('listening on :1234');
})

