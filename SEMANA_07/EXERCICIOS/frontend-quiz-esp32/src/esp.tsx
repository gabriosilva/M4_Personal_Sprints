import axios from "axios";

const BASE_API = "http://ip-do-esp32:80";

export const finishGame = async (
  playerId: number,
  points: number,
  timeInSeconds: number
) => {
  await axios.post(`${BASE_API}/finishPlayer`, {
    playerId,
    points,
    timeInSeconds,
  });
};
