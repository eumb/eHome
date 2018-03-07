import React, { Component } from 'react';
import logo from './logo.svg';
import './App.css';
import {
    Collapse,
    Navbar,
    NavbarToggler,
    NavbarBrand,
    Nav,
    NavItem,
    NavLink,
    Container,
    Row,
    Col,
    Jumbotron,
    Button
} from 'reactstrap';

//import InformationList from './DeviceInformation.js';

import MainBoard from './MainBoard.js';

class App extends Component {
  
constructor(props) {
        super(props);

        this.toggle = this.toggle.bind(this);
        this.state = {
            isOpen: false
        };
    }
    toggle() {
        this.setState({
            isOpen: !this.state.isOpen
        });
    }



  render() {
    return (
      <div className="App">
        
            <Navbar className = "navbar-dark bg-dark" color="inverse" light expand="md">
                    <NavbarBrand href="/">eHome</NavbarBrand>
                    <NavbarToggler onClick={this.toggle} />
                    <Collapse isOpen={this.state.isOpen} navbar>
                        <Nav className="ml-auto" navbar>
                            <NavItem>
                                <NavLink href="/components/">Light</NavLink>
                            </NavItem>
                            <NavItem>
                                <NavLink href="https://github.com/reactstrap/reactstrap">Sensors</NavLink>
                            </NavItem>
                        </Nav>
                    </Collapse>
                </Navbar>
                <Jumbotron>
                    <Container>
                        <Row>
                            <Col>
                                <h1>eHome, your home</h1>
                                <p>
                                Home management, the way it's meant to be
                                </p>
                            </Col>
                        </Row>
                    </Container>
                </Jumbotron>
               {/*<InformationList />*/}
               <MainBoard />
        
      </div>
    );
  }
}

export default App;
