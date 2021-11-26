# copperhammer-scanner-c
**CopperHammer port scanner &amp; exporter, written in C.**

[![Codacy Badge](https://app.codacy.com/project/badge/Grade/7470572aec6b4b53b0c2fe34e58d78f7)](https://www.codacy.com?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=copper-hammer/copperhammer-scanner-c&amp;utm_campaign=Badge_Grade)

## What is CopperHammer
CopperHammer is a similar thing to the Copenheimer project but has a different
purpose, that is, not to grief open servers, but to inform their owners of
possible damage that may happen.

## How CopperHammer works
CopperHammer works by scanning every possible open IPv4 address for public
Minecraft servers by sending to them a [Server List Ping](https://wiki.vg/Server_List_Ping) and reading the response.
If it is a correct [SLP response](https://wiki.vg/Server_List_Ping#Response) and
contains a valid JSON, it will be processed and stored for further investigation.
Server List Ping does not send whitelist status, so we have to connect to
a server. This is a task for another service INSERT LINK HERE.

## Output formats
CopperHammer scanner can output results in different formats, such as JSON
(ready to send to the master node), text (with parsed players list and other
relevant properties), raw data (unprocessed JSON) and a hex dump of an entire
response packet (without reading the length of response or any packet parsing).
Note that when using hexdump, it will **not** check for valid Minecraft Ping
response and that may result in false positives such as SSH, HTTP and FTP
servers.

## Building from source
At first, you need to clone this repository. Note that we're using library
`cJSON` as a submodule, so you have to clone it recursively:
```bash
git clone https://github.com/copper-hammer/copperhammer-scanner-c --recursive
cd copperhammer-scanner-c
```

After cloning, all you have left to do is `make` it:
```bash
make clean all
```

This should result in the final binary in the `./bin/` folder.

This project is not intended for installation, so there is no `make install`.
However, if you really want to, you can just copy it anywhere you want:
```bash
sudo cp ./bin/main /usr/local/bin/copperscan
```
