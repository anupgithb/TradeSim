// import React from "react";
// import { Box, Typography, Paper, Divider } from "@mui/material";

// export function OutputPanel({ metrics }) {
//     return (
//         <Paper elevation={3} sx={{ p: 2, height: "100%" }}>
//             <Typography variant="h6" gutterBottom>
//                 Output Parameters
//             </Typography>
//             <Divider sx={{ mb: 2 }} />

//             <Box sx={{ mb: 1 }}>
//                 <Typography>
//                     <strong>Expected Slippage:</strong> {metrics.slippage.toFixed(4)}
//                 </Typography>
//             </Box>
//             <Box sx={{ mb: 1 }}>
//                 <Typography>
//                     <strong>Expected Fees:</strong> {metrics.fees.toFixed(4)}
//                 </Typography>
//             </Box>
//             <Box sx={{ mb: 1 }}>
//                 <Typography>
//                     <strong>Market Impact:</strong> {metrics.impact.toFixed(4)}
//                 </Typography>
//             </Box>
//             <Box sx={{ mb: 1 }}>
//                 <Typography>
//                     <strong>Net Cost:</strong> {metrics.netCost.toFixed(4)}
//                 </Typography>
//             </Box>
//             <Box sx={{ mb: 1 }}>
//                 <Typography>
//                     <strong>Maker/Taker Proportion:</strong>{" "}
//                     {metrics.makerTakerRatio.toFixed(4)}
//                 </Typography>
//             </Box>
//             <Box>
//                 <Typography>
//                     <strong>Internal Latency:</strong> {metrics.latency.toFixed(2)} ms
//                 </Typography>
//             </Box>
//         </Paper>
//     );
// }

import React from "react";
import { Box, Typography, Paper, Divider } from "@mui/material";

export function OutputPanel({ metrics }) {
    // Safely destructure with defaults to avoid undefined
    const {
        slippage = 0,
        fees = 0,
        impact = 0,
        netCost = 0,
        makerTakerRatio = 0,
        internalLatency = 0,
    } = metrics || {};

    return (
        <Paper elevation={3} sx={{ p: 2, height: "100%" }}>
            <Typography variant="h6" gutterBottom>
                Output Parameters
            </Typography>
            <Divider sx={{ mb: 2 }} />

            <Box sx={{ mb: 1 }}>
                <Typography>
                    <strong>Expected Slippage:</strong> {slippage.toFixed(4)}
                </Typography>
            </Box>
            <Box sx={{ mb: 1 }}>
                <Typography>
                    <strong>Expected Fees:</strong> {fees.toFixed(4)}
                </Typography>
            </Box>
            <Box sx={{ mb: 1 }}>
                <Typography>
                    <strong>Market Impact:</strong> {impact.toFixed(4)}
                </Typography>
            </Box>
            <Box sx={{ mb: 1 }}>
                <Typography>
                    <strong>Net Cost:</strong> {netCost.toFixed(4)}
                </Typography>
            </Box>
            <Box sx={{ mb: 1 }}>
                <Typography>
                    <strong>Maker/Taker Proportion:</strong> {makerTakerRatio.toFixed(4)}
                </Typography>
            </Box>
            <Box>
                <Typography>
                    <strong>Internal Latency (ms):</strong> {internalLatency.toFixed(2)}
                </Typography>
            </Box>
        </Paper>
    );
}
