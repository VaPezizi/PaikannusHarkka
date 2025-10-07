const express = require('express')
const app = express()
const cors = require('cors')

app.use(cors())

app.use(express.json())


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
  response.json(measurements)
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
  console.log("request", request)
  console.log("request body ", request.body)
  console.log("request body content", request.body.content)

  const p = request.body

/*

            m->iTOW,
            m->lon / 10000000.0,
            m->lat / 10000000.0,
            m->height,
            m->hMSL,
            m->hAcc,
            m->vAcc

*/

  if ((p.height === undefined || p.lon === undefined || p.lat === undefined ) !== undefined) {
    return response.status(400).json({ 
      error: 'height, lat or lon missing ...' 
    })
  }  

  const maxId = Math.floor(Math.random() * 1000000)
  p.id = String(maxId + 1)

  persons = persons.concat(p)

  response.json(p)
})

const PORT = 3001
app.listen(PORT, () => {
  console.log(`Server running on port ${PORT}`)
})