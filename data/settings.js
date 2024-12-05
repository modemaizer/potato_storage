function getSettings() {
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function() {
        if (xhr.readyState == XMLHttpRequest.DONE) {
            var response = JSON.parse(xhr.responseText);
            document.getElementById("air_temperature").value = response.airTemperature;
            document.getElementById("air_delta").value = response.airTemperatureDelta;
            document.getElementById("floor_min").value = response.floorTemperatureLow;
            document.getElementById("floor_max").value = response.floorTemperatureHigh;
            document.getElementById("sensors_interval").value = response.sensorsInterval;
            document.getElementById("mqtt_interval").value = response.mqttInterval;
            document.getElementById("version").innerHTML = response.version;
        }
    }
    xhr.open("GET", "/settings/data", true);
    xhr.send();
  }
  
  function handleSubmit(event) {
    event.preventDefault();
    var myform = document.getElementById("myform");
    
    var formData = new FormData(myform);

    fetch("/settings/update", {
      method: "POST",
      body: formData,
    })
    .then(response => {
      if (!response.ok) {
        throw new Error('network returns error');
      }
      return response.json();
    })
    .then((resp) => {
      document.getElementById("air_temperature").value = resp.airTemperature;
      document.getElementById("air_delta").value = resp.airTemperatureDelta;
      document.getElementById("floor_min").value = resp.floorTemperatureLow;
      document.getElementById("floor_max").value = resp.floorTemperatureHigh;
      document.getElementById("sensors_interval").value = resp.sensorsInterval;
      document.getElementById("mqtt_interval").value = resp.mqttInterval;
      document.getElementById("version").innerHTML = resp.version;
      showMessage();
    })
    .catch((error) => {
      console.log("error ", error);
    });
  }

  const fadeIn = (el, timeout, display) => {
    el.style.opacity = 0;
    el.style.transition = `opacity ${timeout}ms`;
    setTimeout(() => {
      el.style.opacity = 1;
    }, 10);
  };

  const fadeOut = (el, timeout) => {
    el.style.opacity = 1;
    el.style.transition = `opacity ${timeout}ms`;
    
    setTimeout(() => {
      el.style.opacity = 0;
    }, timeout);
  };
  function showMessage() {
    var msg = document.getElementById("message");
    fadeIn(msg, 1000, 'flex');
    fadeOut(msg, 1000);
  }