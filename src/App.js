// // import logo from './logo.svg';
// // import './App.css';

// // function App() {
// //   return (
// //     <div className="App">
// //       <header className="App-header">
// //         <img src={logo} className="App-logo" alt="logo" />
// //         <p>
// //           Edit <code>src/App.js</code> and save to reload.
// //         </p>
// //         <a
// //           className="App-link"
// //           href="https://reactjs.org"
// //           target="_blank"
// //           rel="noopener noreferrer"
// //         >
// //           Learn React
// //         </a>
// //       </header>
// //     </div>
// //   );
// // }

// // export default App;


// import React, { useState, useEffect } from "react";
// import { Grid, Container } from "@mui/material";
// import { InputPanel } from "./components/InputPanel"
// import { OutputPanel } from "./components/OutputPanel";

// // Initial default params & metrics
// const defaultParams = {
//   exchange: "OKX",
//   spotAsset: "BTC-USDT",
//   orderType: "market",
//   quantity: 100,
//   volatility: 0.01,
//   feeTier: "Tier 1",
// };

// const defaultMetrics = {
//   slippage: 0,
//   fees: 0,
//   impact: 0,
//   netCost: 0,
//   makerTakerRatio: 0,
//   latency: 0,
// };

// function App() {
//   const [params, setParams] = useState(defaultParams);
//   const [metrics, setMetrics] = useState(defaultMetrics);

//   useEffect(() => {
//     const ws = new WebSocket("ws://localhost:9000/metrics");
//     ws.onmessage = (ev) => {
//       const m = JSON.parse(ev.data);
//       setMetrics(m);
//     };
//     return () => ws.close();
//   }, []);


//   // Handler for input changes
//   const handleChange = (e) => {
//     const { name, value } = e.target;
//     setParams((p) => ({ ...p, [name]: value }));
//   };

//   // TODO: open WebSocket to backend and update metrics in real time
//   useEffect(() => {
//     // Example: fetch initial metrics or connect WS
//     // fetch("/api/metrics", ...).then(...).then(data => setMetrics(data));
//   }, []);

//   return (
//     <Container maxWidth="lg" sx={{ mt: 4, mb: 4 }}>
//       <Grid container spacing={3}>
//         {/* Left Panel */}
//         <Grid item xs={12} md={6}>
//           <InputPanel params={params} onChange={handleChange} />
//         </Grid>

//         {/* Right Panel */}
//         <Grid item xs={12} md={6}>
//           <OutputPanel metrics={metrics} />
//         </Grid>
//       </Grid>
//     </Container>
//   );
// }

// export default App;


import React, { useState, useEffect } from "react";
import { Grid, Container } from "@mui/material";
import { InputPanel } from "./components/InputPanel";
import { OutputPanel } from "./components/OutputPanel";

// Initial default params & metrics
const defaultParams = {
  exchange: "OKX",
  spotAsset: "BTC-USDT",
  orderType: "market",
  quantity: 100,
  volatility: 0.01,
  feeTier: "Tier 1",
};

const defaultMetrics = {
  slippage: 0,
  fees: 0,
  impact: 0,
  netCost: 0,
  makerTakerRatio: 0,
  internalLatency: 0, // corrected key to match C++ struct
};

function App() {
  const [params, setParams] = useState(defaultParams);
  const [metrics, setMetrics] = useState(defaultMetrics);

  useEffect(() => {
    const ws = new WebSocket("ws://localhost:9000/metrics");

    ws.onopen = () => {
      console.log("Connected to backend WebSocket.");
    };

    ws.onmessage = (ev) => {
      const m = JSON.parse(ev.data);
      setMetrics(m);
    };

    ws.onerror = (err) => {
      console.error("WebSocket error:", err);
    };

    return () => ws.close();
  }, []);

  const handleChange = (e) => {
    const { name, value } = e.target;
    setParams((p) => ({ ...p, [name]: value }));
  };

  return (
    <Container maxWidth="lg" sx={{ mt: 4, mb: 4 }}>
      <Grid container spacing={3}>
        {/* Left Panel */}
        <Grid item xs={12} md={6}>
          <InputPanel params={params} onChange={handleChange} />
        </Grid>

        {/* Right Panel */}
        <Grid item xs={12} md={6}>
          <OutputPanel metrics={metrics} />
        </Grid>
      </Grid>
    </Container>
  );
}

export default App;
