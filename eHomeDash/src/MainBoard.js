
import React, {Component} from 'react';
import Device from './Device.js';
import update from 'immutability-helper';

export default class MainBoard extends Component {

 constructor(props) {
    super(props);
    this.state={device1:[],device2:[],device3:[]};


 }
  



componentDidMount(){

    

   fetch(`http://mariusb.go.ro:1280/status`,{
   headers:{
    
    'Access-Control-Allow-Origin':'*'
    },
       cache: 'no-cache', // *default, no-cache, reload, force-cache, only-if-cached
    credentials: 'same-origin', // include, *omit
    mode: 'cors',
        redirect: 'follow', // *manual, error
    referrer: 'no-referrer', // *client
  })
    .then(result=>result.json())
    .then(device1=>this.setState({device1}))

  fetch(`http://mariusb.go.ro:1180/status`,{
    headers:{

    'Access-Control-Allow-Origin':'*'
    },
    cache: 'no-cache', // *default, no-cache, reload, force-cache, only-if-cached
    credentials: 'same-origin', // include, *omit
    mode: 'cors',
        redirect: 'follow', // *manual, error
    referrer: 'no-referrer', // *client
  })
    .then(result=>result.json())
    .then(device2=>this.setState({device2}))
  

    fetch(`http://mariusb.go.ro:1380/status`,{
    headers:{

    'Access-Control-Allow-Origin':'*'
    },
    cache: 'no-cache', // *default, no-cache, reload, force-cache, only-if-cached
    credentials: 'same-origin', // include, *omit
    mode: 'cors',
        redirect: 'follow', // *manual, error
    referrer: 'no-referrer', // *client
  })
    .then(result=>result.json())
    .then(device3=>this.setState({device3}))
  
}

DeviceInfo(device){

  return <Device data={device} />
}



render(){
	

	
	return(
	<div>
	  
  	<div className="row">
  	
    
         {this.DeviceInfo(this.state.device1)}
   
         {this.DeviceInfo(this.state.device2)}
         
         {this.DeviceInfo(this.state.device3)}

    </div>
  </div>
	)
}
}
