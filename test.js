import { xdd } from "./lol.js";

let x = 4;
for (let i = 0; i < 10; i++) {
    x += i;
}

throw new AggregateError([new Error("lol")]);
