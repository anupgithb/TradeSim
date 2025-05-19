// import React, { useState, useEffect } from "react";
// import { Grid, Container } from "@mui/material";
// import { InputPanel } from "./components/InputPanel";
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
//   internalLatency: 0, // corrected key to match C++ struct
// };

// function App() {
//   const [params, setParams] = useState(defaultParams);
//   const [metrics, setMetrics] = useState(defaultMetrics);

//   useEffect(() => {
//     const ws = new WebSocket("ws://localhost:9000/metrics");

//     ws.onopen = () => {
//       console.log("Connected to backend WebSocket.");
//     };

//     ws.onmessage = (ev) => {
//       const m = JSON.parse(ev.data);
//       setMetrics(m);
//     };

//     ws.onerror = (err) => {
//       console.error("WebSocket error:", err);
//     };

//     return () => ws.close();
//   }, []);

//   const handleChange = (e) => {
//     const { name, value } = e.target;
//     setParams((p) => ({ ...p, [name]: value }));
//   };

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


// src/App.js
import React, { useState, useEffect, useRef } from "react";
import { Grid, Container } from "@mui/material";
import { InputPanel } from "./components/InputPanel";
import { OutputPanel } from "./components/OutputPanel";

// Default metrics in case nothing has arrived yet
const defaultMetrics = {
  slippage: 0,
  fees: 0,
  impact: 0,
  netCost: 0,
  makerTakerRatio: 0,
  internalLatency: 0,
};

function App() {
  // Holds the most recent metrics from C++
  const [metrics, setMetrics] = useState(defaultMetrics);

  // Use a ref so we can hand the same WS instance into InputPanel
  const wsRef = useRef(null);

  useEffect(() => {
    // 1) Open WebSocket
    wsRef.current = new WebSocket("ws://localhost:9000/metrics");

    wsRef.current.onopen = () => {
      console.log("▶️ Connected to backend WebSocket");
    };

    wsRef.current.onmessage = (ev) => {
      try {
        const m = JSON.parse(ev.data);
        setMetrics(m);
      } catch (err) {
        console.error("⚠️ Invalid JSON from backend:", ev.data);
      }
    };

    wsRef.current.onerror = (err) => {
      console.error("❌ WebSocket error:", err);
    };

    wsRef.current.onclose = () => {
      console.log("⬅️ WebSocket closed");
    };

    // 2) Cleanup on unmount
    return () => {
      if (wsRef.current) {
        wsRef.current.close();
      }
    };
  }, []); // run once

  return (
    <Container maxWidth="lg" sx={{ mt: 4, mb: 4 }}>
      <Grid container spacing={3} sx={{ display: "flex" }}>
        <Grid item xs={12} sx={{ flex: 2 }}>
          <InputPanel ws={wsRef.current} />
        </Grid>
        <Grid item xs={12} sx={{ flex: 1 }}>
          <OutputPanel metrics={metrics} />
        </Grid>
      </Grid>
    </Container>
  );
}

export default App;
