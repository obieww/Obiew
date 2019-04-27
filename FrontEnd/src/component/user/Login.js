import React, { Component } from 'react';
import { connect } from 'react-redux';
import {
  createNewObiew,
  changePage,
  changeUser,
  changeObiews,
} from '../../store/actions';
import './user.less';

class Login extends Component {
  constructor(props) {
    super(props);
    this.state = {
      username: '',
      password: ''
    }
    this.onUsernameChange = this.onUsernameChange.bind(this);
    this.onPasswordChange = this.onPasswordChange.bind(this);
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

  onSubmit() {
    const {
      password,
      username,
    } = this.state;
    const {
      onChangePage,
      onChangeUser,
    } = this.props;
    fetch('https://sleepy-island-43632.herokuapp.com//api/user/login', {
      method: 'post',
      headers: {'Content-Type': 'application/json'},
      body: JSON.stringify({
        password: password,
        username: username,
      })
    })
    .then(response => response.json())
    .then(user => {
      if (user) {
        onChangeUser(user.username, user.userId);
        onChangePage('home');
        console.log(user.userId);
      }
    })
    .catch(err => {
      console.log(err);
      onChangePage('register');
    })
  }

  render() {
    return (
      <article className="br3 ba dark-gray b--black-10 mv4 w-100 w-50-m w-25-l mw6 shadow-5 center" style={{backgroundColor:'rgba(255,255,255,0.2)'}}>
        <main className="pa4 black-80">
          <div className="measure">
            <legend className="db f2 fw6 ph0 mh0">Log In</legend>
            <fieldset id="sign_up" className="ba b--transparent ph0 mh0">
              <div className="mv3">
                <label className="db fw6 lh-copy f4" htmlFor="email-address">Email</label>
                <input className="pa2 input-reset ba bg-transparent hover-bg-white hover-black w-100" 
                  onChange={this.onUsernameChange}  type="email" name="email-address"  id="email-address"/>
              </div>
              <div className="mv3">
                <label className="db fw6 lh-copy f4" htmlFor="password">Password</label>
                <input className="b pa2 input-reset ba bg-transparent hover-bg-white hover-black w-100" 
                  onChange={this.onPasswordChange}  type="password" name="password"  id="password"/>
              </div>
            </fieldset>
            <div className="db mv3">
              <input className="b ph3 pv2 input-reset ba b--black bg-transparent grow pointer f4 dib br2" 
                onClick={this.onSubmit} type="submit" value="Login"/>
            </div>
          </div>
        </main>
      </article>
    );
  }
}

const mapStateToProps = state => ({
  obiews: state.obiews,
  username: state.username,
});

const mapDispatchToProps = dispatch => ({
  onCreateNewObiew: obiew => dispatch(createNewObiew(obiew)),
  onChangePage: page => dispatch(changePage(page)),
  onChangeUser: (username, userId) => dispatch(changeUser(username, userId)),
});

export default connect(mapStateToProps, mapDispatchToProps)(Login)