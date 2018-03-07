import React, { Component, PropTypes } from 'react';
 
// Task component - represents a single todo item
export default class InfoValues extends Component {
  render() {
  	sensortype=this.props.sensor;
  	//console.log(sensortype)
  	//console.log(this.props.sensorvalue)
    
  	
    return (
      <p> <strong> Updated at: </strong>{this.props.sensorvalue.created_at.toString()}</p>
    );
  }

}
 