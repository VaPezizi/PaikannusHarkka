import {
    MapContainer,
    TileLayer,
    Marker,
    Popup,
    Polyline
} from 'react-leaflet';

import 'leaflet/dist/leaflet.css';
import './App.css';
import { useState, useEffect } from 'react';
import axios from 'axios';
import io from 'socket.io-client'
import { ClipLoader } from 'react-spinners'; //Found this online :D

const ShowLocationsInList = ({locations}) => {
  //Using mongodbs _id as key
  return (
    <div className="locations-container">
      <h2>Locations</h2>
      <ul>
        {locations.map((location) => (
          <li key={location._id}>
            <p>Time: {location.time}, Latitude: {location.latitude}, Longitude: {location.longitude}</p>
          </li>
        ))}
      </ul>
    </div>
  )
}



const ShowMap = ({locations}) => {

  //console.log("Show locations: ", locations);

  const validLocations = locations.filter((location) => location && location.latitude !== undefined && location.longitude !== undefined);
  // Create an array of [latitude, longitude] pairs for the Polyline
  const positions = validLocations.map(location => [location.latitude, location.longitude]);

  return (
    <div className="map-container">
      <MapContainer
        center={[62.7903, 22.8406]}
        zoom={6}
        style={{height: "400px"}}
        >
        <TileLayer
          attribution='&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors'
          url="https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"
        />
        <Polyline positions={positions} color="blue" />
      </MapContainer>
    </div>
  )
}


const App = () => {
  const [locations, setLocations] = useState([]);
  const [loading, setLoading] = useState(true); // Track loading state


  useEffect(() => {
    console.log('useEffect');

    const fetchInitialLocations = async () => {
      try {
        const response = await axios.get('http://localhost:3001/api/measurements');
        console.log('Fetched initial locations:', response.data);

        setLocations(response.data.measurements || []); // Ensure it's an array
        setLoading(false); // Data fetched successfully
      } catch (error) {
        console.error('Error fetching initial locations:', error);
        setLoading(false); // Stop loading even if there's an error
      }
    };

    fetchInitialLocations();

    // Connect to Socket.IO server
    const socket = io('http://localhost:3001');

    // Handle new locations being added
    socket.on('locationAdded', (newLocation) => {
      console.log('New location added:', newLocation);
      setLocations((prevLocations) => [...prevLocations, newLocation]);
    });

    // Cleanup on unmount
    return () => {
      socket.disconnect();
    };
  }, []);

  const clearDb = async () => {
    try {
      await axios.delete('http://localhost:3001/api/measurements/destroy');
      console.log('Database cleared');
      setLocations([]); // Clear the locations state
    } catch (error) {
      console.error('Error clearing the database:', error);
    }
  };

  return (
    <div className="App">
      {loading ? (
        <ClipLoader color="#FFFFFF" loading={loading} size={50}/> 
      ) : (
          <>
            <ShowMap locations={locations} />
            <div className='buttons-container'>
              <button className="clear-db-button" onClick={clearDb}>Clear Database</button>
              
            </div>
          <ShowLocationsInList locations={locations} />
        </>
      )}
    </div>
  );
};
export default App