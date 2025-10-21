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

const ShowLocationsInList = ({ locations, toggleSelection }) => {
  //Using mongodbs _id as key

  //Just realised that the iTow time, means GPS time of week...
  //Unfortunately, i don't have time to implement the UTC conversion, so it'll have to do :(

  //https://gssc.esa.int/navipedia/index.php/Transformations_between_Time_Systems
  /*const formatTime = (timestamp) => {
    //const date = new Date(timestamp);
    const GPS_EPOCH = new Date(Date.UTC(1980, 0, 6, 0, 0, 0, 0));
    const date = new Date(GPS_EPOCH.getTime() + timestamp);

    return date.toLocaleString();
  }*/
  //console.log(formatTime(locations[0]))

  return (
    <div className="locations-container">
      <h2>Locations</h2>
      <ul>
        {locations.map((location) => (
          <li key={location._id}>
            
            <p>Time: {location.time}, Latitude: {location.latitude}, Longitude: {location.longitude}, Height: {location.height}mm</p>
            <input type='checkbox' name={"Checbox" + location._id} onChange={() => toggleSelection(location._id)}/>
          
          </li>
        ))}
      </ul>
    </div>
  )
}

const Buttons = ({ clearDb, clearSelected, sortBy, exportToCSV }) => {

  return(
    <div className='buttons-container'>
      <button className="clear-db-button" onClick={clearDb}>Clear Database</button>
      <button className="clear-db-button" onClick={clearSelected}>Clear Selected</button>
      <button className="button" onClick={() => sortBy('time')}>Sort by Time</button>
      <button className="button" onClick={() => sortBy('latitude')}>Sort by Latitude</button>
      <button className="button" onClick={() => sortBy('longitude')}>Sort by Longitude</button>
      <button className="button" onClick={() => sortBy('height')}>Sort by Height</button>
      <button className="export-button" onClick={() => exportToCSV()}>Export to CSV</button>
      
    </div>
  )
}


const ShowMap = ({locations}) => {

  //console.log("Show locations: ", locations);
  //Last minute rubber band fix to not mess up the map rendering with a sorted list
  const validLocations = locations
    .filter((location) => location && location.latitude !== undefined && location.longitude !== undefined)
    .sort((a, b) => a.time - b.time); // Sort by time
  // Create an array of [latitude, longitude] pairs for the Polyline
  //validLocations.sortBy = (a, b) => a.time - b.time;
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
  const [selectedLocations, setSelected] = useState([]);
  const [currentSort, setCurrentSort] = useState("time");
  const [currentSortOrder, setCurrentSortOrder] = useState("asc");

  useEffect(() => {
    console.log('useEffect');
  /*
    const updateBothLocations = (newLocation) => { 
      setLocations((prevLocations) => [...prevLocations, [newLocation]]);
      setDrawPositions((prevLocations) => [...prevLocations, newLocation]);
      console.log();
    }
    */

    const fetchInitialLocations = async () => {
      try {
        const response = await axios.get('http://localhost:3001/api/measurements');
        console.log('Fetched initial locations:', response.data);

        const locationsTemp = response.data.measurements || [] // Ensure it's an array
        setLocations(locationsTemp); 
        //setDrawPositions(locationsTemp.map(location => [location.latitude, location.longitude]));
        //updateBothLocations(locationsTemp);
        //updateBothLocations(response.data.measurements || []);
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
      //setDrawPositions((prevLocations) => [...prevLocations, [newLocation.latitude, newLocation.longitude]]);
      //updateBothLocations(newLocation)
    });

    return () => {
      socket.disconnect();
    };
  }, []);
  const exportToCSV = () => {
    if (locations.length === 0) {
      console.log("No locations to export.");
      return;
    }
  
    // Define the CSV header
    const headers = ["Time", "Latitude", "Longitude", "Height"];
    
    // Map locations to CSV rows
    const rows = locations.map((location) => [
      location.time,
      location.latitude,
      location.longitude,
      location.height,
    ]);
  
    // Combine headers and rows into a CSV string
    const csvContent = [headers, ...rows]
      .map((row) => row.join(",")) // Join each row with commas
      .join("\n"); // Join rows with newlines
  
    // Create a Blob and download the file
    const blob = new Blob([csvContent], { type: "text/csv;charset=utf-8;" });
    const url = URL.createObjectURL(blob);
    const link = document.createElement("a");
    link.href = url;
    link.setAttribute("download", "measurements.csv");
    document.body.appendChild(link);
    link.click();
    document.body.removeChild(link);
  };
  /*
  useEffect(() => {
    console.log('Selected locations updated:', selectedLocations);
  }, [selectedLocations]);
  */
  const clearDb = async () => {
    try {
      await axios.delete('http://localhost:3001/api/measurements/destroy');
      console.log('Database cleared');
      setLocations([]); // Clear the locations state
    } catch (error) {
      console.error('Error clearing the database:', error);
    }
  };
  const clearSelected = async () => {
    try {
      await Promise.all(
        selectedLocations.map((locationId) => {
          axios.delete("http://localhost:3001/api/measurements/" + locationId)
          console.log("Deleting location with id: ", locationId);
        })
      );

      setLocations((prevLocations) => {
        return prevLocations.filter((location) => !selectedLocations.includes(location._id));
      });
      setDrawPositions((prevLocations) => {
        return prevLocations.filter((location) => !selectedLocations.includes(location._id));
      });
      setSelected([]); // Clear selected 


    } catch (error) {
      console.error('Error clearing the database:', error);
    }
  };

  const toggleSelection = (locationId) => {
    setSelected((selectedLocations) => {
      if (selectedLocations.includes(locationId)) {
        return selectedLocations.filter((_id) => _id !== locationId);
      } else {
        return [...selectedLocations, locationId];
      }
    });
    //console.log(selectedLocations)
  };

  const sortBy = (key) => {
    let sortedLocations = [];
    let newSortOrder = "asc";
    if (key === currentSort) {
      //If same button is clicked twice, reverse the order
      newSortOrder = currentSortOrder === "asc" ? "desc" : "asc"; 
      //setCurrentSortOrder(currentSortOrder === "asc" ? "desc" : "asc"); //<----I did this at first, and boy was it pain debug
      setCurrentSortOrder(newSortOrder);
      if (newSortOrder === "asc") {
        sortedLocations = [...locations].sort((a, b) => {
          if (a[key] > b[key]) return -1;
          if (a[key] < b[key]) return 1;
        
          return 0;
        });
      }else {
        sortedLocations = [...locations].sort((a, b) => {
          if (a[key] < b[key]) return -1;
          if (a[key] > b[key]) return 1;
          return 0;
        });
      }
    } else {
      setCurrentSortOrder("asc");
      sortedLocations = [...locations].sort((a, b) => {
        if (a[key] < b[key]) return -1;
        if (a[key] > b[key]) return 1;
        return 0;
      });
    }
    
    setCurrentSort(key);
    setLocations(sortedLocations);
  };

  /*const sortLocationsByTime = () => {
    const sortedLocations = [...locations].sort((a, b) => a.time - b.time);
    setLocations(sortedLocations);
  }*/

  return (
    <div className="App">
      {loading ? (
        <ClipLoader color="#FFFFFF" loading={loading} size={50}/> 
      ) : (
          <>
            <h1>Samulin hullu paikannus sivu</h1>
            <ShowMap locations={locations} />
            <Buttons clearDb={clearDb} clearSelected={clearSelected} sortBy={sortBy} exportToCSV={exportToCSV} />
          <ShowLocationsInList locations={locations} toggleSelection={toggleSelection}/>
        </>
      )}
    </div>
  );
};
export default App