import express, { json } from 'express';
import cors from 'cors';
import { createServer } from 'http';
import { Server } from 'socket.io';
import mongoose from 'mongoose';
import dotenv from 'dotenv';
import { time } from 'console';

dotenv.config();

const app = express();
const server = createServer(app);
const io = new Server(server, {
  cors: {
    origin: "http://localhost:5173",
    methods: ["GET", "POST"]
  }
});
app.use(cors());
app.use(express.json())

const mongoURI = process.env.MONGO_URI;
console.log("Connecting to MongoDB at ", mongoURI);
mongoose.set('strictQuery', false);
mongoose.connect(mongoURI)

const measurementSchema = new mongoose.Schema({
  id: String,
  latitude: Number,
  longitude: Number,
  height: Number,
  time: Number
});

const Measurement = mongoose.model('Measurement', measurementSchema);

//const measurement = new Measurement({ id: '1', latitude: 12.3456, longitude: 12.3456, height: 123, time: 123456});

let measurements = []


/*

struct UBX_NAV_HPPOSLLH_load
            m->iTOW,
            m->lon / 10000000.0,
            m->lat / 10000000.0,
            m->height,
            m->hMSL,
            m->hAcc,
            m->vAcc
*/



/*
let persons = [
  {
    id: "1",
    name: "Petteri",
    number: "12345"
  },
  {
    id: "2",
    name: "Hannu",
    number: "34567"
  },  
]
*/
/*
app.get('/', (request, response) => {
  response.send('<h1>Hello World!</h1>')
})

app.get('/info', (request, response) => {
  let info = {
    infoText: "Phonebook has info of " + persons.length + " people",
    datetimeText: new Date()
  }
  response.json(info)
})
  */

app.get('/api/measurements', async (request, response) => {
  /*response.json({
    success: true,
    data: measurements,
    count: measurements.length
  })
  */
  try {
    const measurements = await Measurement.find({});
    response.json({
      measurements
    });

  } catch (error) {
    response.status(500).json({
      success: false,
      message: error.message
    });
  }
});

app.get('/api/measurements/:id', async (request, response) => {
  try {
    const id = request.params.id;
    //const m = measurements.find(p => p.id === id);
    const m = await Measurement.findById(id);
  
    if (m) {
      response.json(m);
    } else {
      response.status(404).end();
    }

  } catch (error) {
    response.json({success: false, message: error.message});
  }
})

// This shouldn't be used in production, but it fits this project
app.delete('/api/measurements/destroy', async (request, response) => {
  try {
    const result = await Measurement.deleteMany({});
    response.status(204).end();
  }catch (error) {
    response.status(500).json({
      success: false,
      message: error.message
    });
  }
});
app.delete('/api/measurements/:id', async (request, response) => {
  try {
    const id = request.params.id;
    //measurements = measurements.filter(p => p.id !== id);
    const result = await Measurement.findByIdAndDelete(id);

    if (result) { // if found and deleted succesfully
      response.status(204).end();
    } else {
      response.status(404).json({
        success: false,
        message: 'Measurement not found'
      });
    }

  }catch (error) {
    response.status(500).json({
      success: false,
      message: error.message
    });
  }
})



app.post('/api/measurements', async (request, response) => {
  //console.log("request body ", request.body);
  try {
    const p = request.body;
    console.log(p)

    if ((p.height === undefined || p.lon === undefined || p.lat === undefined)) {
      return response.status(400).json({
        success: false,
        message: 'Invalid location data'
      })
    }

    console.log("request body content", request.body.content);
    const newMeasurement = new Measurement({
      latitude: p.lat,
      longitude: p.lon,
      height: p.height,
      time: p.iTOW,
    });
    const savedMeasurement = await newMeasurement.save();

    /*
    
                m->iTOW,
                m->lon / 10000000.0,
                m->lat / 10000000.0,
                m->height,
                m->hMSL,
                m->hAcc,
                m->vAcc
    
    */
    io.emit('locationAdded', newMeasurement);
    return response.status(201).json({
      success: true,
      data: savedMeasurement
    });
  } catch (error) {
    return response.status(500).json({
      success: false,
      message: error.message
    });
  }

})

const PORT = 3001
server.listen(PORT, () => {
  console.log(`Server running on port ${PORT}`)
})