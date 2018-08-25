const momment = require('moment');
const _ = require('lodash/collection');
const plotly = require('plotly')('fol21', 'y32TAWwBymoFOnT0vkvE');
const {
    MqttSubscriber,
    MongoDataClient
} = require('t4l-console-susbcriber');

const conf = require('./resources/config.json');
const monitor = new MqttSubscriber({
    host: conf.mqtt.host,
    port: conf.mqtt.port,
    topic: conf.mqtt.ds_topic
});

let dbName = 't4l_test2'
let url = 'mongodb://localhost:27017/' + dbName;
const client = new MongoDataClient(url, dbName);

let count = 0;
let graphIndex = 1;

monitor.onMessage((topic, message) => {
    console.log(message.toString())
    message = JSON.parse(message.toString());
    client.start().then((client) => {
        client.insertOne(message);
        if (count >= 100) {
            count = 0;
            client.collectData(100).then((chunk) => {
                let timeseries = _.map(_.map(chunk, 'timestamp'), (ts) => {
                    return momment(ts).format("YYYY-MM-DD HH:mm:ss");
                });
                let temps = _.map(chunk, 'data.main');

                var data = [{
                    x: timeseries,
                    y: temps,
                    type: "scatter"
                }];

                var graphOptions = {
                    filename: "cpu-temp_"+ graphIndex,
                    fileopt: "overwrite"
                };
                plotly.plot(data, graphOptions, function (err, msg) {
                    if(err)
                        console.log(err);
                    else 
                    {
                        console.log(msg);
                        graphIndex++;
                    }
                });
            });
        }
    })
    count++;
})

monitor.init();

// const plotly = require('plotly')('fol21', 'y32TAWwBymoFOnT0vkvE');

// var data = [
//   {
//     x: ["2013-10-04 22:23:00", "2013-11-04 22:23:00", "2013-12-04 22:23:00"],
//     y: [1, 3, 6],
//     type: "scatter"
//   }
// ];
// var graphOptions = {filename: "date-axes", fileopt: "overwrite"};
// plotly.plot(data, graphOptions, function (err, msg) {
//     console.log(msg);
// });