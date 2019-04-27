import React, {Component} from "react";
import Post from "./card/Post.js";
import Card from "../container/CardContainer.js";
import Portrait from '../images/person_default.png';
import { connect } from 'react-redux';
import {
  createNewObiew,
} from '../store/actions';
import utils from "../utils";
import "./MainPanel.less";

class MainPanel extends Component {
  constructor(props) {
    super(props);
    this.state = {
      showAnchor: false,
    };
    this.onMousewheel = this.onMousewheel.bind(this);
    this.onCreateNewObiew = this.onCreateNewObiew.bind(this);
  }

  componentWillMount() {
    const {
      userId,
      username,
      onChangeObiews,
    } = this.props
    fetch(utils.defaultUrl + '/api/obiew/randomfeed', {
      method: 'GET',
      headers: {'Content-Type': 'application/json'},
    })
    .then(response => response.json())
    .then(response => {
      onChangeObiews(
        response.map(obiew => ({
          userId: userId,
          obiewId: obiew.obiewId,
          username: obiew.username,
          timestamp: Date.now(),
          likes: obiew.likeList.map(like =>({
            userId: userId,
            obiewId: obiew.obiewId, 
          })), 
          comments: obiew.commentList.map(comment => ({
            obiewId: obiew.obiewId,
            userId: userId,
            username: comment.username,
            timestamp: Date.now(),
            reply: comment.content,
          })), 
          reobiews: [],
          obiew: obiew.content,
        })
      );
    })
    .catch(err => console.log(err));
    window.addEventListener("scroll", this.onMousewheel);
  }

  componentWillUnmount() {
    window.removeEventListener("scroll", this.onMousewheel);
  }

  onMousewheel() {
    this.setState({
      showAnchor: window.scrollY >= 50
    });
  }

  onCreateNewObiew(msg) {
    const {
      userId,
      obiews,
      username,
      onCreateNewObiew,
    } = this.props;
    fetch(utils.defaultUrl + '/api/obiew/post', {
      method: 'post',
      headers: {'Content-Type': 'application/json'},
      body: JSON.stringify({
        content: msg,
        user: {
          userId: userId
        }
      })
    })
    .then(response => {
      onCreateNewObiew({
        userId: userId,
        obiewId: Date.now(),
        username: username,
        timestamp: Date.now(),
        likes: [], 
        comments: [], 
        reobiews: [],
        obiew: msg,
      });
    })
    .catch(err => console.log(err));
  }

  render() {
    const {
      showAnchor,
    } = this.state;
    const {
      obiews,
      username,
      profile_portrait,
    } = this.props;
    return (
      <div className="container main-container">
        <div className="main-content">
          <div className="row">
            <div className="col-md-2 right-panel br3 ba dark-gray b--black-10 shadow-5">
              <div className="center">
                <div className="tc">
                  <div className='tc'>
                    <input className='upload-file f4 pa2 w-70 center' type='tex' />
                    <img src={Portrait} className="br-100 h4 w4 dib ba b--black-05 pa2" alt="profile" />
                  </div>
                  <h1 className="f3 mb2">{username}</h1>
                  <h2 className="f5 fw4 gray mt0">{"obiews: " + obiews.length}</h2>
                </div>
              </div>
            </div>
            <div className="col-md-8">
              <Post
                msg="Tell the world what's happening"
                onClick={this.onCreateNewObiew} />
              {
                obiews.map(obiew => <Card key={obiew.obiewId} obiew={obiew} />)
              }
              {
                showAnchor ?
                <div
                  className="anchor" 
                  v-show="showAnchor"
                  onClick={() => window.scrollTo(0, 0)}>
                  <span className="glyphicon glyphicon-chevron-up" />
                </div> :
                null
              }
            </div>
          </div>
        </div>
      </div>
    );
  }
}

const mapStateToProps = state => ({
  obiews: state.obiews,
  username: state.username,
  userId: state.userId,
});

const mapDispatchToProps = dispatch => ({
  onCreateNewObiew: obiew => dispatch(createNewObiew(obiew)),
});

export default connect(mapStateToProps, mapDispatchToProps)(MainPanel)