# Server Power Management

A set software components designed for managing computers / servers with support for Wake On LAN protocol and IPMI interfaces






### Software components
- Library: Contains most of the functions for detecting and sending power on / off signals via lan
- Frontent: The easy to use GUI that provides a simple table for easy management of each computer / server. It imports the SPM library at runtime (Powered by [WxWidgets](https://www.wxwidgets.org/))
- Service daemon: The service that only runs on linux and listens for LAN singnals (power off, get user, ip, broadcast ip, mac addres)
- CLI: Provides a small command line inteerface along with a builtin shell
- Web page: A web page interface that fully resembles the frontend version. This feature is provided in the CLI;  




* Features
- [X] Basic wake on lan function
- [X] Device list
- [X] Power off along side wake on lan via server listener (In progress)
- [ ] Automatic detection of avaialble computers / servers (Works only on windows)
- [ ] Storage environment (In progress)
- [ ] Power Status Check
- [ ] Frontend GUI (Already in rpogress)
- [ ] Restricted session
- [ ] Linux daemon service (In progress)
- [ ] Process black list
- [ ] Headless Command Line execution
- [ ] stdout buffer capture to text file
- [ ] Command Line interface
- [ ] Saving power statistics
- [ ] Custom json written device / server lists
- [ ] Local web page setup
- [ ] C/C++ API examples (In progress)

* Compatibility
- [X] Wake On LAN protocol
- [ ] IPMI
- [ ] Security features via OpenSSL


### This project is still in early development...
