import React, {Component} from 'react';
import humanizeDuration from 'humanize-duration';

export default class Device extends Component {


restartDevice(device,e){
var url;
function handleClick(e) {
    e.preventDefault();
    console.log('The link was clicked.');
  }
url="http://"+device+"/restart";

console.log(url);
//window.open(url);
fetch(url, {
  method: 'POST',
  mode: 'no-cors', // no-cors, *same-origin
    redirect: 'follow', // *manual, error
    referrer: 'no-referrer', // *client
}).then(console.log("device restarted"))


}

updateDevice(device,e){
var url;
function handleClick(e) {
    e.preventDefault();
    console.log('The link was clicked.');
  }
url="http://"+device+"/";
console.log(url);
window.open(url);
/*fetch(url, {
  method: 'GET',
  mode: 'no-cors', // no-cors, *same-origin
    redirect: 'follow', // *manual, error
    referrer: 'no-referrer', // *client
}).then(console.log("device updating"))*/


}



refresh(e){
  function handleClick(e) {
    e.preventDefault();
    console.log('The link was clicked.');
  }
  this.forceUpdate();

}

formatTime(device){
  return humanizeDuration(device,
{ 
units: ['d', 'h', 'm'] ,

round:true

});

}
render(){
  this.formatTime();
  var style = {
      width:'20rem',
      'color':'white',
      'backgroundColor': '#343a40',
      'borderColor': '#343a40'
    };

console.log(this.props.data);

return(
	<div>
	
	<div className = "container-fluid">
  <div className="row">
    <div className="mycard col-sm-1"></div>

	  <div className="col-sm-3">
	  	<div className="card card-inverse card-primary mb-4 text-left" style={style}>
		  <div className="card-block">
		    <h4 className="card-title text-center">{this.props.data.deviceScope}</h4>

  <table className="table">

  <tbody>
    <tr>
    
      <td>DeviceId</td>

      <td>{this.props.data.deviceId}</td>
    </tr>
     <tr>
    
      <td>Device Type</td>

      <td>{this.props.data.deviceType}</td>
    </tr>

      <tr>
    
      <td>Device Location</td>

      <td>{this.props.data.deviceLocation}</td>
    </tr>

     <tr>
    
      <td>Device Scope</td>

      <td>{this.props.data.deviceScope}</td>
    </tr>
  
      <tr>
    
      <td>IP address</td>

      <td>{this.props.data.IP}</td>
    </tr>


    <tr>

      <td>Heap size</td>
    
      <td>{this.props.data.heap}</td>
    </tr>
    <tr>

      <td>Wifi strength</td>
  
      <td>{this.props.data.wifistr}</td>
    </tr>

{/*    <tr>
      <th>Sw state</th>
  	<tr>
      <td>Sw1</td>
      <td>{this.props.data.sW1_state}</td>
    </tr>
	<tr>
	  <td>Sw2</td>
      <td>{this.props.data.sW2_state}</td>
    </tr>
    <tr>
      <td>Sw3</td>
      <td>{this.props.data.sW3_state}</td>
    </tr>
    </tr>*/}

	<tr>

      <td>Uptime</td>
  
      <td>{this.formatTime(this.props.data.uptime)}</td>
    </tr>

 {/*   <tr>

      <td>Restart reason</td>
  
      <td>{this.props.data.lastReason}</td>
    </tr>*/}

  </tbody>
</table>
		
	
			 </div>
      <button onClick={(e) => this.restartDevice(this.props.data.IP,e)} className="btn btn-primary">Restart</button>
	  <button onClick={(e) => this.updateDevice(this.props.data.IP,e)} className="btn btn-primary">Update</button>

		</div>
	  </div>

  </div>
  </div>
  </div>
  )
}
}

