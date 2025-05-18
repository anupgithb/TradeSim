#!/usr/bin/env bash
# kill.sh — stop the running TradeSim.exe by reading its PID

PIDFILE="./run.pid"

if [[ ! -f "$PIDFILE" ]]; then
  echo "No PID file found; is TradeSim running?"
  exit 1
fi

pid=$(<"$PIDFILE")
if ! kill -0 "$pid" &>/dev/null; then
  echo "Process $pid not running; removing stale PID file."
  rm -f "$PIDFILE"
  exit 0
fi

# Send SIGTERM, wait up to 5s, then SIGKILL if needed
kill "$pid"
echo "Sent SIGTERM to PID $pid. Waiting up to 5s..."
for i in {1..5}; do
  if kill -0 "$pid" &>/dev/null; then
    sleep 1
  else
    break
  fi
done

if kill -0 "$pid" &>/dev/null; then
  echo "Still running after 5s; sending SIGKILL."
  kill -9 "$pid"
else
  echo "Process $pid exited cleanly."
fi

rm -f "$PIDFILE"

