const SerialPort = require('serialport');
const Readline = require('@serialport/parser-readline');

const port =  new SerialPort('/dev/ttyACM0', { baudRate: 9600 });

const request = require("request")
const parser = port.pipe(new Readline({ delimiter: '\n' }));

const SERVER_URL = "http://127.0.0.1:5000/items/update"


console.log("start node-client")

port.on('open', () => {
    console.log('serial port open');
});

port.on('close', () => {
    console.log('serial port closed');
});


function handler(data) {
    console.log("=======================================================");
    console.log('data: ', data);
    console.log("=======================================================");

    try {

        const params = data.slice(1, -2).split('/');
        // console.log(params[1], params[2]);
        const [ day, month, year] = params[1].split('.');
        const [ hours, minutes, seconds ] = params[2].split(':');
        // console.log(day, month, year, hours, minutes, seconds);

        const datetime = new Date(year, month - 1, day, hours, minutes, seconds);
        
        console.log(datetime);

        const info = {
            'id': parseInt(params[0], 16),
            'time':  datetime.toISOString(),
            'location': params[3], 
        };

        console.log(info);
        

        request.post(
            SERVER_URL,
            { json: info },
            function (error, response, body) {
                if (!error && response.statusCode == 200) {
                    console.log("- - - - - - - - - - - - - - - - - - - - - - - - - - -");
                    console.log(body)
                } else {
                    console.error("Server is unavailable")
                }
            }
        );
    } catch(err) {
        console.error(err);
    }


}

parser.on('data', handler);

