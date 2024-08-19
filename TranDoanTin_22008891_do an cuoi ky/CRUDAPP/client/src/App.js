import "./App.css";
import { useState } from "react";
import Axios from "axios";

function App() {
  const [device, setDevice] = useState("");
  const [temperature, setTemperature] = useState(0);
  const [humidity, setHumidity] = useState("");
 
  const [newWage, setNewWage] = useState(0);

  const [nhietdoList, setNhietDoList] = useState([]);

  const addNhietDo = () => {
    Axios.post("http://localhost:3001/create", {
      device: device,
      temperature: temperature,
      humidity: humidity,
    }).then(() => {
      setNhietDoList([
        ...nhietdoList,
        {
          device: device,
          temperature: temperature,
          humidity: humidity,
          
        },
      ]);
    });
  };

  const getNhietDo = () => {
    Axios.get("http://localhost:3001/nhietdo").then((response) => {
      setNhietDoList(response.data);
    });
  };

  const updateNhietDoTemperature = (id) => {
    Axios.put("http://localhost:3001/update", { wage: newWage, id: id }).then(
      (response) => {
        setNhietDoList(
          nhietdoList.map((val) => {
            return val.id === id
              ? {
                  id: val.id,
                  device: val.device,
                  temperature: val.temperature,
                  humidity: val.humidity,
                }
              : val;
          })
        );
      }
    );
  };

  const deleteNhietDo = (id) => {
    Axios.delete(`http://localhost:3001/delete/${id}`).then((response) => {
      setNhietDoList(
        nhietdoList.filter((val) => {
          return val.id !== id;
        })
      );
    });
  };

  return (
    <div className="App">
      <div className="information">
        <label>Device:</label>
        <input
          type="text"
          onChange={(event) => {
            setDevice(event.target.value);
          }}
        />
        <label>Temperature:</label>
        <input
          type="number"
          onChange={(event) => {
            setTemperature(event.target.value);
          }}
        />
        <label>Humid:</label>
        <input
          type="text"
          onChange={(event) => {
            setHumidity(event.target.value);
          }}
        />
       
        <button onClick={addNhietDo}>Add nhietdo</button>
      </div>
      <div className="nhietdo">
        <button onClick={getNhietDo}>Show nhietdo</button>

        {nhietdoList.map((val, key) => {
          return (
            <div className="employee">
              <div>
                <h3>Device: {val.device}</h3>
                <h3>Temperature: {val.temperature}</h3>
                <h3>Humidity: {val.humidity}</h3>
              </div>
              <div>
                <input
                  type="text"
                  placeholder="2000..."
                  onChange={(event) => {
                    setNewWage(event.target.value);
                  }}
                />
                <button
                  onClick={() => {
                    updateNhietDoTemperature(val.temperature);
                  }}
                >
                  {" "}
                  Update
                </button>

                <button
                  onClick={() => {
                    deleteNhietDo(val.temperature);
                  }}
                >
                  Delete
                </button>
              </div>
            </div>
          );
        })}
      </div>
    </div>
  );
}

export default App;
