import React, { useEffect, useState } from "react";
import { Text, View, StyleSheet } from "react-native";
import Ionicons from "@expo/vector-icons/Ionicons";
import AsyncStorage from "@react-native-async-storage/async-storage";
import init from "react_native_mqtt";
import axios from "axios"; // Import axios for making HTTP requests

init({
  size: 10000,
  storageBackend: AsyncStorage,
  defaultExpires: 1000 * 3600 * 24,
  enableCache: true,
  sync: {},
});

const options = {
  host: "broker.emqx.io",
  port: 8083,
  path: "/tin_thcntt3",
  id: "id_" + parseInt(Math.random() * 100000),
};

const client = new Paho.MQTT.Client(options.host, options.port, options.path);

const TurnOnOffLedScreen_Mqtt = ({ navigation }) => {
  const [msg, setMsg] = useState("No message");
  const [temperature, setTemperature] = useState(null);
  const [humidity, setHumidity] = useState(null);

  useEffect(() => {
    connect();
    client.onMessageArrived = onMessageArrived;
  }, []);

  const connect = () => {
    client.connect({
      onSuccess: () => {
        console.log("connect MQTT broker ok!");
        subscribeTopic();
      },
      useSSL: false,
      timeout: 5,
      onFailure: () => {
        console.log("connect fail");
        connect();
        console.log("reconnect ...");
      },
    });
  };

  const subscribeTopic = () => {
    client.subscribe("tin/led", { qos: 0 });
  };

  const onMessageArrived = async (message) => {
    console.log("onMessageArrived:" + message.payloadString);
    setMsg(message.payloadString);
    const jsondata = JSON.parse(message.payloadString);
    console.log(jsondata.message);
    setTemperature(jsondata.temperature); // Assuming temperature and humidity are part of the received JSON
    setHumidity(jsondata.humidity);

    // Sending data to MySQL
    try {
      const response = await axios.post("http://localhost:3001/create", {
        temperature: jsondata.temperature,
        humidity: jsondata.humidity,
      });
      console.log("Data sent to MySQL:", response.data);
    } catch (error) {
      console.error("Error sending data to MySQL:", error);
    }
  };

  return (
    <View style={styles.containerLedView}>
      <View style={styles.header}>
        <Ionicons name="home-outline" size={64} color="orange" />
        <Text style={styles.title}>Nhiet do, do am</Text>
        <Text style={styles.subTitle}>ON / OFF LIGHT</Text>
      </View>
      <View style={styles.main}>
        <View style={styles.boxLightOff}>
          <Ionicons name="thermometer-outline" size={64} color="grey" />
        </View>
        <Text style={styles.subTitle}>{msg}</Text>
        <Text style={styles.subTitle}>
          {temperature && humidity ? JSON.stringify({ temperature, humidity }) : "No data"}
        </Text>
      </View>
    </View>
  );
};

const styles = StyleSheet.create({
  containerLedView: {
    flex: 1,
    backgroundColor: "#FFF",
  },
  header: {
    alignItems: "center",
    justifyContent: "center",
    padding: 20,
  },
  title: {
    fontSize: 24,
    fontWeight: "bold",
    marginTop: 10,
    color: "#FFA500",
  },
  subTitle: {
    fontSize: 16,
    color: "#333",
  },
  main: {
    flex: 1,
    justifyContent: "center",
    alignItems: "center",
    padding: 20,
  },
  boxLightOff: {
    backgroundColor: "#E0E0E0",
    padding: 20,
    borderRadius: 10,
    marginBottom: 20,
  },
});

export default TurnOnOffLedScreen_Mqtt;
