// import React from "react";
// import {
//     Box,
//     Typography,
//     TextField,
//     FormControl,
//     InputLabel,
//     Select,
//     MenuItem,
//     Paper,
// } from "@mui/material";

// export function InputPanel({ params, onChange }) {
//     return (
//         <Paper elevation={3} sx={{ p: 2, height: "100%" }}>
//             <Typography variant="h6" gutterBottom>
//                 Input Parameters
//             </Typography>

//             <FormControl fullWidth margin="dense">
//                 <InputLabel>Exchange</InputLabel>
//                 <Select
//                     name="exchange"
//                     value={params.exchange}
//                     label="Exchange"
//                     onChange={onChange}
//                 >
//                     <MenuItem value="OKX">OKX</MenuItem>
//                 </Select>
//             </FormControl>

//             <TextField
//                 fullWidth
//                 margin="dense"
//                 name="spotAsset"
//                 label="Spot Asset"
//                 value={params.spotAsset}
//                 onChange={onChange}
//                 placeholder="BTC-USDT"
//             />

//             <FormControl fullWidth margin="dense">
//                 <InputLabel>Order Type</InputLabel>
//                 <Select
//                     name="orderType"
//                     value={params.orderType}
//                     label="Order Type"
//                     onChange={onChange}
//                 >
//                     <MenuItem value="market">Market</MenuItem>
//                 </Select>
//             </FormControl>

//             <TextField
//                 fullWidth
//                 margin="dense"
//                 name="quantity"
//                 type="number"
//                 label="Quantity (USD)"
//                 value={params.quantity}
//                 onChange={onChange}
//             />

//             <TextField
//                 fullWidth
//                 margin="dense"
//                 name="volatility"
//                 label="Volatility"
//                 value={params.volatility}
//                 onChange={onChange}
//             />

//             <TextField
//                 fullWidth
//                 margin="dense"
//                 name="feeTier"
//                 label="Fee Tier"
//                 value={params.feeTier}
//                 onChange={onChange}
//             />
//         </Paper>
//     );
// }
// src/components/InputPanel.jsx
import React, { useState } from "react";
import {
    Box,
    Paper,
    Typography,
    TextField,
    Select,
    MenuItem,
    Button,
    FormControl,
    InputLabel
} from "@mui/material";

export function InputPanel({ ws }) {
    const [exchange, setExchange] = useState("OKX");
    const [spotAsset, setSpotAsset] = useState("BTC-USDT");
    const [orderType, setOrderType] = useState("market");
    const [quantity, setQuantity] = useState(100);
    const [volatility, setVolatility] = useState(0.01);
    const [feeTier, setFeeTier] = useState("Tier 1");

    const handleSubmit = () => {
        const params = { exchange, spotAsset, orderType, quantity, volatility, feeTier };
        ws.send(JSON.stringify({ type: "settings", params }));
    };

    return (
        <Paper sx={{ p: 2 }}>
            <Typography variant="h6" gutterBottom>Input Parameters</Typography>
            <Box display="flex" flexDirection="column" gap={2}>
                <TextField label="Exchange" value={exchange} onChange={e => setExchange(e.target.value)} />
                <TextField label="Spot Asset" value={spotAsset} onChange={e => setSpotAsset(e.target.value)} />
                <FormControl>
                    <InputLabel>Order Type</InputLabel>
                    <Select value={orderType} onChange={e => setOrderType(e.target.value)}>
                        <MenuItem value="market">Market</MenuItem>
                        <MenuItem value="limit">Limit</MenuItem>
                    </Select>
                </FormControl>
                <TextField
                    label="Quantity (USD)"
                    type="number"
                    value={quantity}
                    onChange={e => setQuantity(+e.target.value)}
                />
                <TextField
                    label="Volatility"
                    type="number"
                    inputProps={{ step: 0.001 }}
                    value={volatility}
                    onChange={e => setVolatility(+e.target.value)}
                />
                <FormControl>
                    <InputLabel>Fee Tier</InputLabel>
                    <Select value={feeTier} onChange={e => setFeeTier(e.target.value)}>
                        <MenuItem value="Tier 1">Tier 1</MenuItem>
                        <MenuItem value="Tier 2">Tier 2</MenuItem>
                        <MenuItem value="Tier 3">Tier 3</MenuItem>
                    </Select>
                </FormControl>
                <Button variant="contained" onClick={handleSubmit}>Submit</Button>
            </Box>
        </Paper>
    );
}
