import express, { json } from 'express';
import cors from 'cors';
import { createServer } from 'http';
import { Server } from 'socket.io';


const app = express();
const server = createServer(app);
const io = new Server(server, {
  cors: {
    origin: "http://localhost:5173",
    methods: ["GET", "POST"]
  }
});
app.use(cors());
app.use(json())


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

app.get('/api/measurements', (request, response) => {
  response.json({
    success: true,
    data: measurements,
    count: measurements.length
  })
})

app.get('/api/measurements/:id', (request, response) => {
  const id = request.params.id
  const m = measurements.find(p => p.id === id)
  
  if (m) {
    response.json(m)
  } else {
    response.status(404).end()
  }
})

app.delete('/api/measurements/:id', (request, response) => {
  const id = request.params.id
  measurements = measurements.filter(p => p.id !== id)

  response.status(204).end()
})

app.post('/api/measurements', (request, response) => {
  console.log("request body ", request.body)
  const p = request.body
  console.log("request body content", request.body.content)

/*

            m->iTOW,
            m->lon / 10000000.0,
            m->lat / 10000000.0,
            m->height,
            m->hMSL,
            m->hAcc,
            m->vAcc

*/

  if ((p.height === undefined || p.lon === undefined || p.lat === undefined )) {
    return response.status(400).json({ 
      success: false,
      messahe: 'Invalid location data'
    })
  }  

  const maxId = Math.floor(Math.random() * 1000000)
  p.id = String(maxId + 1)
  const testi = {
    latitude: p.lat,
    longitude: p.lon,
    height: p.height,
    time: p.iTOW,
  }

  measurements.push(testi);
  io.emit('locationAdded', testi);
  return response.status(201).json({
    success: true,
    data: p
  })

})

const PORT = 3001
server.listen(PORT, () => {
  console.log(`Server running on port ${PORT}`)
})