#!/usr/bin/env bash
# run.sh — launch TradeSim.exe in a fresh log directory, backing up the old one

BINARY="./TradeSim.exe"
PIDFILE="./run.pid"
LOG_ROOT="./logs"
CURRENT_LOG="$LOG_ROOT/current"
TIMESTAMP=$(date '+%Y%m%d_%H%M%S')

# 1. Backup previous logs if they exist
if [[ -d "$CURRENT_LOG" ]]; then
  mkdir -p "$LOG_ROOT"
  mv "$CURRENT_LOG" "$LOG_ROOT/$TIMESTAMP"
  echo "Backed up logs/current → logs/$TIMESTAMP"
fi

# 2. Create fresh log directory
mkdir -p "$CURRENT_LOG"

# 3. Check that the binary exists
if [[ ! -x "$BINARY" ]]; then
  echo "Error: $BINARY not found or not executable."
  exit 1
fi

# 4. Prevent double‑runs
if [[ -f "$PIDFILE" ]]; then
  oldpid=$(<"$PIDFILE")
  if kill -0 "$oldpid" &>/dev/null; then
    echo "TradeSim is already running (PID $oldpid)."
    exit 0
  else
    echo "Removing stale PID file."
    rm -f "$PIDFILE"
  fi
fi

# 5. Start the process, redirect stdout/stderr to the new log
LOGFILE="$CURRENT_LOG/simulator.log"
echo "Starting TradeSim — logging to $LOGFILE"
nohup "$BINARY" >"$LOGFILE" 2>&1 &

# 6. Record its PID
echo $! >"$PIDFILE"
echo "Started TradeSim (PID $!)"

