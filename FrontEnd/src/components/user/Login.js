import React, { Component } from 'react';

class Login extends Component {
  constructor(props) {
    super(props);
    this.state = {
      username: '',
      password: ''
    }
  }

  onEmailChange(e) {
    this.setState({
      username: e.target.value
    })
  }

  onPasswordChange(e) {
    this.setState({
      password: e.target.value
    })
  }

  onSubmitSignIn() {
  }

  render() {
    return (
      <article className="br3 ba dark-gray b--black-10 mv4 w-100 w-50-m w-25-l mw6 shadow-5 center" style={{backgroundColor:'rgba(255,255,255,0.2)'}}>
        <main className="pa4 black-80">
          <div className="measure">
            <legend className="db f2 fw6 ph0 mh0">Sign In</legend>
            <fieldset id="sign_up" className="ba b--transparent ph0 mh0">
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
            </fieldset>
            <div className="db mv3">
              <input className="b ph3 pv2 input-reset ba b--black bg-transparent grow pointer f4 dib br2" 
                onClick={this.onSubmitSignIn} type="submit" value="Sign In"/>
            </div>
          </div>
        </main>
      </article>
    );
  }
}

export default Login;