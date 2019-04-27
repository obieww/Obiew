import MainPanel from '../component/MainPanel.js';
import { connect } from 'react-redux';
import {
  changeObiews,
  createNewObiew,
} from '../store/actions';

const mapStateToProps = state => ({
  userId: state.userId,
  obiews: state.obiews,
  username: state.username,
});

const mapDispatchToProps = dispatch => ({
  onCreateNewObiew: obiew => dispatch(createNewObiew(obiew)),
  onChangeObiews: obiews => dispatch(changeObiews(obiews)),
});

export default connect(mapStateToProps, mapDispatchToProps)(MainPanel)
