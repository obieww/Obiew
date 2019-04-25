import React, { Component } from 'react';
import './user.less'

class Register extends Component {
  constructor(props) {
    super(props);
    this.state = {
      username: '',
      password: '',
      fisrtname: '',
      lastname: '',
      email: '',
      phone: '',
    }
    this.onUsernameChange = this.onUsernameChange.bind(this);
    this.onPasswordChange = this.onPasswordChange.bind(this);
    this.onFirstnameChange = this.onFirstnameChange.bind(this);
    this.onLastnameChange = this.onLastnameChange.bind(this);
    this.onEmailChange = this.onEmailChange.bind(this);
    this.onPhoneChange = this.onPhoneChange.bind(this);
    this.onSubmit = this.onSubmit.bind(this);
  }

  onUsernameChange(e) {
    this.setState({
      username: e.target.value
    })
  }

  onPasswordChange(e) {
    this.setState({
      password: e.target.value
    })
  }

  onFirstnameChange(e) {
    this.setState({
      fisrtname: e.target.value
    })
  }

  onLastnameChange(e) {
    this.setState({
      lastname: e.target.value
    })
  }

  onEmailChange(e) {
    this.setState({
      email: e.target.value
    })
  }

  onPhoneChange(e) {
    this.setState({
      phone: e.target.value
    })
  }

  onSubmit() {
    const {
      email,
      password,
      username,
    } = this.state;
    fetch('https://secure-eyrie-24516.herokuapp.com/api/user/register', {
      method: 'post',
      headers: {'Content-Type': 'application/json'},
      body: JSON.stringify({
        email: email,
        password: password,
        username: username,
      })
    })
    .then(response => response.json())
    .then(user => {
      console.log(user)
    })
  }

  render() {
    return (
      <article className="br3 ba dark-gray b--black-10 mv4 w-100 w-50-m w-25-l mw6 shadow-5 center">
          <main className="pa4 black-80">
            <div className="measure">
              <legend className="db f2 fw6 ph0 mh0">Register</legend>
              <fieldset id="sign_up" className="ba b--transparent ph0 mh0">
                <div className="mv3">
                  <label className="db fw6 lh-copy f4" htmlFor="username">Username</label>
                  <input className="pa2 input-reset ba bg-transparent hover-bg-white hover-black w-100" 
                    onChange={this.onUsernameChange}  type="username" name="username"  id="username"/>
                </div>
                <div className="mv3">
                  <label className="db fw6 lh-copy f4" htmlFor="email-address">Email</label>
                  <input className="pa2 input-reset ba bg-transparent hover-bg-white hover-black w-100" 
                    onChange={this.onEmailChange}  type="email" name="email-address"  id="email-address"/>
                </div>
                <div className="mv3">
                  <label className="db fw6 lh-copy f4" htmlFor="password">Password</label>
                  <input className="b pa2 input-reset ba bg-transparent hover-bg-white hover-black w-100" 
                    onChange={this.onPasswordChange}  type="password" name="password"  id="password"/>
                </div>
                <div className="mv3">
                  <label className="db fw6 lh-copy f4" htmlFor="password">First Name</label>
                  <input className="b pa2 input-reset ba bg-transparent hover-bg-white hover-black w-100" 
                    onChange={this.onFirstnameChange}  type="firstname" name="firstname"  id="fisrtname"/>
                </div>
                <div className="mv3">
                  <label className="db fw6 lh-copy f4" htmlFor="password">Last Name</label>
                  <input className="b pa2 input-reset ba bg-transparent hover-bg-white hover-black w-100" 
                    onChange={this.onLastnameChange}  type="lastname" name="lastname"  id="lastname"/>
                </div>
                <div className="mv3">
                  <label className="db fw6 lh-copy f4" htmlFor="password">Phone Number</label>
                  <input className="b pa2 input-reset ba bg-transparent hover-bg-white hover-black w-100" 
                    onChange={this.onPhoneChange}  type="phone" name="phone"  id="phone"/>
                </div>
              </fieldset>
              <div className="db mv3">
                <input className="b ph3 pv2 input-reset ba b--black bg-transparent grow pointer f4 dib br2" 
                  onClick={this.onSubmit} type="submit" value="Register"/>
              </div>
            </div>
          </main>
      </article>
    );
  }
}

export default Register;