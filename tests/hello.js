import data from "./data.js"

print(data);

class xdd {
    constructor() {
        this.nice = "nice"
    }
}

function xd() { return xd; }

print(
    { hello: {
        ano: true
    }}
)

print(xd);

print(xdd);

let foo = new Map();
foo.set(xd, "yes");
foo.set("lol", foo);
foo.set(foo, "haha");
foo.set(xdd, new Map([[xd, xdd]]));

print(foo);
